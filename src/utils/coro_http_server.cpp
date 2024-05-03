
#include <thread>
#include <boost/url/url.hpp>
#include "coro_http_server.h"
#include <netinet/in.h>
#include "logger.h"

using namespace boost::beast;
awaitable<void> CoroHTTPServer::session_handler(tcp::socket socket) {
    try {
        boost::beast::flat_buffer buffer(8196);
        for (;;) {
            http::request<http::string_body> request;
            co_await http::async_read(socket, buffer, request, use_awaitable);
            boost::urls::url url(request.target());
            auto methods = _routes.find(url.path());
            if (methods != nullptr) {
                auto function = methods->find(request.method());
                if (function != methods->end()) {
                    http::response<http::string_body> response = co_await function->second(socket,request);
                    co_await http::async_write(socket, std::move(response), use_awaitable);
                } else { 
                    //NOT_MODIFIED
                    log_trace("Method: " << request.method() << " Url: " << url.path() << " NOT_MODIFIED");
                    http::response<http::string_body> response = http::response<http::string_body>{http::status::not_modified, request.version()};
                    response.set(http::field::content_type, "text/html");
                    response.keep_alive(request.keep_alive());
                    co_await http::async_write(socket, std::move(response), use_awaitable);
                }
            } else {
                //NOT_FOUND
                log_trace("Method: " << request.method() << " Url: " << url.path() << " NOT_FOUND");
                http::response<http::string_body> response = http::response<http::string_body>{http::status::not_found, request.version()};
                response.set(http::field::content_type, "text/html");
                response.keep_alive(request.keep_alive());
                co_await http::async_write(socket, std::move(response), use_awaitable);
            }
            buffer.clear();
            if (!_keepAlive || !request.keep_alive()) {
                break;
            }
        }
    } catch (boost::system::system_error & ex1) {
     
    } catch (std::exception& ex0) {
        log_warn(ex0.what());
    }
    socket.shutdown(tcp::socket::shutdown_send);
    co_return;
}
CoroHTTPServer::CoroHTTPServer(size_t threads,bool keepAlive): _keepAlive(keepAlive) {
    for (int i = 0; i < threads; i++) {
        _subservices.emplace_back();
    }
};
CoroHTTPServer::~CoroHTTPServer() {

};
int CoroHTTPServer::start(uint16_t port) {
    std::condition_variable cv;
    std::mutex mtx;
    bool started = false;
    
    for (auto &service : _subservices) {
        service.io_context = std::make_shared<boost::asio::io_context>();
        service.thread = std::make_unique<std::thread>([io_context = service.io_context,port,this] () {
            tcp::acceptor acceptor(*io_context);
            tcp::endpoint endpoint = tcp::endpoint(tcp::v4(), port);
            acceptor.open(tcp::v4());
            // acceptor.set_option(tcp::acceptor::reuse_address(true));
            // acceptor.bind(endpoint);

            int on = 1;
            setsockopt(acceptor.native_handle(), SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
            setsockopt(acceptor.native_handle(), SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on));
            struct sockaddr_in sa;
            sa.sin_family = AF_INET;
            sa.sin_addr.s_addr = INADDR_ANY;
            sa.sin_port = htons(port);
            bind(acceptor.native_handle(),(const sockaddr *)&sa,sizeof(sa));
            acceptor.listen();
            
            co_spawn(*io_context,[&,io_context,port] () -> awaitable <void> {
                for (;;) {
                    tcp::socket socket = co_await acceptor.async_accept(use_awaitable);
                    co_spawn(socket.get_executor(), this->session_handler(std::move(socket)), detached);
                }
            } , detached);

            io_context->run();
        });
    }

    // std::unique_lock<std::mutex> lock(mtx);
    // log_info("HTTP Server [" << port << "] 正在启动");
    // if (cv.wait_for(lock, std::chrono::seconds(3),[&]() { return started; })) {
    //     io_context = std::move(ioc);
    //     thread = std::move(t);
    //     return 0;
    // } else {
    //     log_info("HTTP Server [" << port << "] 启动超时");
    //     ioc->stop();
    //     t->join();
    //     return -1;
    // }
    return 0;
}

void CoroHTTPServer::stop() {
    for (auto &service : _subservices) {
        if (service.io_context != nullptr && service.thread != nullptr && service.thread->joinable()) {
            log_info("正在关闭");
            service.io_context->stop();
            service.thread->join();
        } else {
            log_error("未启动");
        }
    }
}

void CoroHTTPServer::add(http::verb method, std::string path, HttpFunction function) {
    auto methods = this->_routes.find(path);
    if (methods == nullptr) {
        std::shared_ptr<HttpMethods> methods = std::make_shared<HttpMethods>();
        methods->insert({method,std::move(function)});
        _routes.insert(path,std::move(methods));
    } else {
        auto item = methods->find(method);
        if (item == methods->end()) {
            methods->insert({method,function});
        } else {
            log_warn(method << " " << path << " already exists");
        }
    }
}

CoroHTTPServer&& CoroHTTPServer::getTestInstance() {
    static CoroHTTPServer httpd(4,true);
    httpd.add(http::verb::get, "/", 
            [](auto& socket,auto& request) -> awaitable<http::response<http::string_body>> {
            http::response<http::string_body> response = http::response<http::string_body>{ http::status::ok, request.version() };
            response.version(request.version());
            response.result(http::status::ok);
            response.body() = "Hello World";
            response.set(http::field::server, BOOST_BEAST_VERSION_STRING);
            response.set(http::field::content_type, "text/html");
            response.keep_alive(request.keep_alive());
            response.prepare_payload();
            log_info("/");
            co_return response;
        }
    );
    httpd.add(http::verb::get, "/123", 
            [](auto& socket,auto& request) -> awaitable<http::response<http::string_body>> {
            http::response<http::string_body> response = http::response<http::string_body>{ http::status::ok, request.version() };
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
    httpd.start(10086);
    return std::move(httpd);
}