
#include "http_rpc_module.h"
#include <boost/asio/buffer.hpp>
#include <boost/asio/co_spawn.hpp>
#include <chrono>


namespace module {
HttpRpcModule::HttpRpcModule() = default;
HttpRpcModule::~HttpRpcModule() = default;

void HttpRpcModule::init(std::string_view configure) {
	threads_ = 4;
	pop_balance_ = 0;
	for (int i = 0; i < threads_; i++) {
		i_ringbufs_.emplace_back(std::make_unique<CircularQueue<ModuleRequest>>(1024L));
		o_ringbufs_.emplace_back(std::make_unique<CircularQueue<ModuleRequest>>(1024L));
	}

	service_ = std::make_unique<utils::boost::coroutine::http::Server>(
		utils::boost::coroutine::http::Server(threads_, [&]
			(::std::size_t tid, auto &socket, request<string_body> &request) -> ::boost::asio::awaitable<response<string_body>> {
			GeneralService::Request general_request;
			general_request.set_path(static_cast<std::string_view>(request.target()));
			general_request.set_method(static_cast<std::string_view>(request.method_string()));
			general_request.set_body(static_cast<std::string_view>(request.body()));

			::boost::promise<GeneralService::Response> promise;
			auto future = promise.get_future();
            boost::asio::steady_timer timer(socket.get_executor(), std::chrono::seconds (3L));

			ModuleRequest module_request(std::move(general_request), [&promise](GeneralService::Response &&response) {
				promise.set_value(std::move(response));
			});

			if (i_ringbufs_[tid]->push(std::move(module_request))) {
                int count = 0;
                while (!future.is_ready()) {
                    timer.expires_after(std::chrono::microseconds (1L));
                    co_await timer.async_wait(boost::asio::use_awaitable);
                    if (count++ > 3000) {
                        break;
                    }
                }

				if (future.is_ready()) {
					GeneralService::Response general_response = future.get();
					response < string_body > resp = response < string_body > {status::ok, request.version()};
					resp.body() = "Are U OK\n";
					resp.set(field::content_type, "application/json");
					resp.keep_alive(request.keep_alive());
					resp.prepare_payload();
					co_return resp;
				} else {
					response < string_body > resp =
						response < string_body > {status::gateway_timeout, request.version()};
					resp.set(field::server,
							 BOOST_BEAST_VERSION_STRING);
					resp.body() = "Gateway Timeout";
					resp.keep_alive(false);
					resp.prepare_payload();
					co_return resp;
				}
			} else {
				response < string_body > resp = response < string_body > {
					status::too_many_requests,
					request.version()};
				resp.set(field::server,
						 BOOST_BEAST_VERSION_STRING);
				resp.body() = "Gateway Busy";
				resp.keep_alive(false);
				resp.prepare_payload();
				co_return resp;
			}
		}));
}

void HttpRpcModule::start() {
	service_->start(10086);
}

void HttpRpcModule::stop() {
	service_->stop();
}

void HttpRpcModule::exit() {

}

std::shared_ptr<ModuleRequest> HttpRpcModule::pop() {
	pop_balance_ = (pop_balance_ + 1) % threads_;
	return i_ringbufs_[pop_balance_]->pop();
}
}
