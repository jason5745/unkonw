#include "http_rpc_module.h"

namespace module {
HttpRpcModule::HttpRpcModule() {}
HttpRpcModule::~HttpRpcModule() {}

void HttpRpcModule::init(std::string_view configure) {
    threads_ = 4;
    pop_balance_ = 0;
    for (int i = 0; i < threads_; i++) {
        i_ringbufs_.emplace_back(std::make_unique<RingBuf<std::unique_ptr<ModuleRequest>>>(1024));
        o_ringbufs_.emplace_back(std::make_unique<RingBuf<std::unique_ptr<ModuleRequest>>>(1024));
    }

    service_ = std::make_unique<utils::boost::coroutine::http::Server>(
        utils::boost::coroutine::http::Server(4, [&](int num, auto &socket, request<string_body> &request) ->
            boost::asio::awaitable<response<string_body>> {

            GeneralService::Request general_request;
            general_request.set_path(std::string(request.target()));
            general_request.set_method(std::string(request.method_string()));
            general_request.set_body(request.body());

            std::promise<GeneralService::Response> promise;
            std::future future = promise.get_future();
            std::unique_ptr<ModuleRequest> module_request = std::make_unique<ModuleRequest>(
                std::move(general_request),
                [&promise](GeneralService::Response &&response) {
                    promise.set_value(std::move(response));
                });

            if (i_ringbufs_[num]->push(std::move(module_request))) {
                int timeout = 3000;
                while (
                    future.wait_for(std::chrono::microseconds(0)) != std::future_status::ready) {
                    boost::asio::steady_timer timer(
                        socket.get_executor(),
                        std::chrono::milliseconds(1)
                    );
                    co_await timer.async_wait(boost::asio::use_awaitable);
                    if (--timeout < 0) {
                        break;
                    }
                }
                if (future.wait_for(std::chrono::microseconds(0)) == std::future_status::ready) {
                    GeneralService::Response general_response = future.get();
                    response < string_body > resp = response < string_body > {status::ok, request.version()};
                    resp.body() = std::move("Are U OK\n");
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
        }, true));
}

void HttpRpcModule::start() {
    service_->start(10086);
}

void HttpRpcModule::stop() {
    service_->stop();
}

void HttpRpcModule::exit() {

}

bool HttpRpcModule::pop(std::unique_ptr<ModuleRequest> &request) {
    for (int i = 0; i < threads_; i++) {
        pop_balance_ = (pop_balance_ + 1) % threads_;
        if (i_ringbufs_[pop_balance_]->pop(request)) {
            return true;
        }
    }
    return false;
}

}
