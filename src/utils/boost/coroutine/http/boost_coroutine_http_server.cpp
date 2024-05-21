
#include <thread>
#include <boost/url/url.hpp>
#include <netinet/in.h>
#include "logger.h"
#include "boost_coroutine_http_server.h"

namespace utils {
namespace boost {
namespace coroutine{
namespace http {
awaitable<void> Server::session(WorkHandler *worker, tcp::socket socket) {
    worker->members++;
    try {
        ::boost::beast::flat_buffer buffer(8196);
        for (;;) {
            http::request<http::string_body> request;
            (void) co_await http::async_read(socket, buffer, request, use_awaitable);
            if (global_ != nullptr) {
                http::response<http::string_body> response = co_await global_(worker->tid, socket, request);
                (void) co_await http::async_write(socket, response, use_awaitable);
            } else {
                ::boost::urls::url url(request.target());
                auto methods = routes_.find(url.path());
                if (methods != nullptr) {
                    auto function = methods->find(request.method());
                    if (function != methods->end()) {
                        http::response<http::string_body> response = co_await function->second(worker->tid, socket, request);
                        (void) co_await http::async_write(socket, response, use_awaitable);
                    } else {
                        //NOT_MODIFIED
                        log_trace("Method: {}, Url: {} -> NOT_MODIFIED", request.method_string().data(), url.path());
                        http::response<http::string_body> response = http::response<http::string_body>{
                            http::status::not_modified, request.version()};
                        response.set(http::field::content_type, "text/html");
                        request.keep_alive(false);
                        response.keep_alive(request.keep_alive());
                        (void) co_await http::async_write(socket, response, use_awaitable);
                    }
                } else {
                    //NOT_FOUND
                    log_trace("Method: {}, Url: {} -> NOT_FOUND", request.method_string().data(), url.path());
                    http::response<http::string_body> response = http::response<http::string_body>{
                        http::status::not_found, request.version()};
                    response.set(http::field::content_type, "text/html");
                    request.keep_alive(false);
                    response.keep_alive(request.keep_alive());
                    (void) co_await http::async_write(socket, response, use_awaitable);
                }
            }
            buffer.clear();
            if (!request.keep_alive()) {
                break;
            }
        }
    } catch (::boost::system::system_error &ex1) {
    } catch (std::exception &ex0) {
        log_warn(ex0.what());
    }
    socket.shutdown(tcp::socket::shutdown_send);
    worker->members--;
    co_return;
}

Server::Server(size_t threads) {
    global_ = nullptr;
    workers_ = std::vector<WorkHandler>(threads);
}

Server::Server(size_t threads, HttpFunction global)
    : global_(std::move(global)) {
    workers_ = std::vector<WorkHandler>(threads);
}

Server::~Server() {

}

int Server::start(uint16_t port) {
    int i = 0;
    for (auto &worker : workers_) {
        worker.tid = i++;
        worker.th = std::move(std::thread([&, port]() {

            tcp::acceptor acceptor(worker.ctx);
            acceptor.open(tcp::v4());

            // acceptor.set_option(tcp::acceptor::reuse_address(true));
            // acceptor.bind(tcp::endpoint(tcp::v4(), port));

            int on = 1;
            (void) setsockopt(acceptor.native_handle(), SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on));
            struct sockaddr_in sa {
                .sin_family = AF_INET,
                .sin_port = htons(port),
                .sin_addr {
                    .s_addr = INADDR_ANY
                }
            };
            (void) bind(acceptor.native_handle(), (const sockaddr *) &sa, sizeof(sa));
            acceptor.listen();

            (void) co_spawn(acceptor.get_executor(), [&]() -> awaitable<void> {
                for(;;) {
                    auto it = std::min_element(
                        workers_.begin(), workers_.end(),
                        [](const WorkHandler &l,const WorkHandler &r) {
                            return l.members < r.members;
                        }
                    );
                    if (it != workers_.end()) {
                        tcp::socket socket(it->ctx);
                        co_await acceptor.async_accept(socket,use_awaitable);
                        co_spawn(it->ctx.get_executor(), session(it.base(),std::move(socket)), detached);
                    } else {
                        tcp::socket socket = co_await acceptor.async_accept(use_awaitable);
                        co_spawn(socket.get_executor(), session(&worker,std::move(socket)), detached);
                    }
                }
            }, detached);
            worker.ctx.run();
        }));
    }
    return 0;
}

void Server::stop() {
    for (auto &worker : workers_) {
        if (worker.th.joinable()) {
            log_info("正在关闭");
            worker.ctx.stop();
            worker.th.join();
        } else {
            log_error("未启动");
        }
    }
}

void Server::mount(http::verb method, std::string path, HttpFunction function) {
    auto methods = this->routes_.find(path);
    if (methods == nullptr) {
        std::shared_ptr<HttpMethods> methods = std::make_shared<HttpMethods>();
        methods->insert({method, std::move(function)});
        routes_.insert(path, std::move(methods));
    } else {
        auto item = methods->find(method);
        if (item == methods->end()) {
            methods->insert({method, function});
        } else {
            log_warn("{}", "已添加路由");
        }
    }
}

Server &&Server::getTestInstance() {
    static Server httpd(8);
    httpd.mount(http::verb::get, "/",
                   [](int num, auto &socket, auto &request) -> awaitable<http::response<http::string_body>> {
                       std::future<std::string> future;
                       future.get();
                       // boost::asio::post(socket.get_executor(),);
                       http::response<http::string_body> response = http::response < http::string_body > {
                           http::status::ok,
                           request.version()
                       };
                       response.body() = "Hello World";
                       response.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                       response.set(http::field::content_type, "text/html");
                       response.keep_alive(request.keep_alive());
                       response.prepare_payload();
                       //log_info("/");
                       co_return response;
                   }
    );
    httpd.mount(http::verb::get, "/123",
                   [](int num, auto &socket, auto &request) -> awaitable<http::response<http::string_body>> {
                       http::response<http::string_body>
                           response = http::response < http::string_body > {http::status::ok,
                                                                            request.version()};
                       response.version(request.version());
                       response.result(http::status::ok);
                       response.body() = "Are You OK";
                       response.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                       response.set(http::field::content_type, "text/html");
                       response.keep_alive(request.keep_alive());
                       response.prepare_payload();
                       log_info("/123");
                       co_return response;
                   }
    );
    return std::move(httpd);
}
}
}
}
}