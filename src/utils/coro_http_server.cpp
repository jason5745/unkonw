
#include <thread>
#include <boost/url/url.hpp>
#include "coro_http_server.h"
#include "logger.h"

using namespace boost::beast;
awaitable<void> CoroHTTPServer::session_handler(tcp::socket socket) {
    boost::beast::flat_buffer buffer;
    try {
        for (;;) {
            http::request<http::string_body> request;
            co_await http::async_read(socket, buffer, request, use_awaitable);
            boost::urls::url url(request.target());
            auto methods = routes.find(url.path());
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
            if (!keepAlive || !request.keep_alive()) {
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

int CoroHTTPServer::start(short port,int hint) {
    std::condition_variable cv;
    std::mutex mtx;
    bool started = false;
    
    std::shared_ptr<boost::asio::io_context> ioc = std::make_shared<boost::asio::io_context>(hint);
    std::unique_ptr<std::thread> t = std::make_unique<std::thread>([&,port,ioc]() {
        co_spawn(*ioc,[&]() -> awaitable<void> {
            tcp::acceptor acceptor4(*ioc, tcp::endpoint(tcp::v4(), port));
            for (;;) {
                tcp::socket socket = co_await acceptor4.async_accept(use_awaitable);
                co_spawn(socket.get_executor(), this->session_handler(std::move(socket)), detached);
            }
        } , detached);
    
        // co_spawn(*ioc,[&]() -> awaitable<void> {
        // tcp::acceptor acceptor6(*ioc, tcp::endpoint(tcp::v6(), port));
        //     for (;;) {
        //         tcp::socket socket = co_await acceptor6.async_accept(use_awaitable);
        //         co_spawn(socket.get_executor(), this->session_handler(std::move(socket)), detached);
        //     }
        // } , detached);
        
        log_info("HTTP Server [" + std::to_string(port) + "] 已启动");
        started = true;
        cv.notify_one();
        ioc->run();
        log_info("HTTP Server [" + std::to_string(port) + "] 已停止");
    });

    std::unique_lock<std::mutex> lock(mtx);
    log_info("HTTP Server [" + std::to_string(port) + "] 正在启动");
    if (cv.wait_for(lock, std::chrono::seconds(3),[&]() { return started; })) {
        io_context = std::move(ioc);
        thread = std::move(t);
        return 0;
    } else {
        log_info("HTTP Server [" + std::to_string(port) + "] 启动超时");
        ioc->stop();
        t->join();
        return -1;
    }
}

void CoroHTTPServer::stop() {
    log_info("正在关闭");
    if (io_context != nullptr && thread != nullptr && thread->joinable()) {
        io_context->stop();
        thread->join();
    } else {
        log_error("未启动");
    }
}

void CoroHTTPServer::add(http::verb method, std::string path, HttpFunction function) {
    auto methods = this->routes.find(path);
    if (methods == nullptr) {
        std::shared_ptr<HttpMethods> methods = std::make_shared<HttpMethods>();
        methods->insert({method,std::move(function)});
        routes.insert(path,std::move(methods));
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
    static CoroHTTPServer httpd(100);
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
            co_return response;
        }
    );
    httpd.start(10086,100);
    return std::move(httpd);
}