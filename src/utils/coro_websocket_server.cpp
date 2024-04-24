


#include <condition_variable>
#include <boost/url/url.hpp>
#include "coro_websocket_server.h"
#include "logger.h"

using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;

namespace websocket = boost::beast::websocket;
namespace http = boost::beast::http;

awaitable<void> CoroWebSocketServer::session_handler(tcp::socket socket) {
    boost::beast::flat_buffer buffer;
    bool isConnected = false;
    http::request<http::string_body> request;
    std::shared_ptr<CoroWebSocketHandler> handler = nullptr;
    try { 
        co_await http::async_read(socket, buffer, request, use_awaitable);
        boost::urls::url url(request.target());
        handler = this->routes.find(url.path());
        if (handler == nullptr) {               //判断是否已注册过路由
            http::response<http::string_body> response = http::response<http::string_body>{http::status::not_found, request.version()};
            co_await http::async_write(socket, response, use_awaitable);
            co_return;
        }
        if (!websocket::is_upgrade(request)) {  //判断是否为Websocket握手请求
            http::response<http::string_body> response = http::response<http::string_body>{http::status::not_modified, request.version()};
            co_await http::async_write(socket, response, use_awaitable);
            co_return;
        }

        // TODO: Auth
    } catch (std::exception& ex0) {
        log_warn(ex0.what());
        co_return;
    }
    //开始Websocket会话
    websocket::stream<tcp::socket> client(std::move(socket));
    try {
        co_await client.async_accept(request, use_awaitable);
        isConnected = true;
        if (handler->on_connected != nullptr) {
            co_await handler->on_connected(client);
        }
        for (;;) {
            std::size_t n = co_await client.async_read(buffer, use_awaitable);
            if (handler->on_message != nullptr) {
                co_await handler->on_message(client, (char *)buffer.cdata().data(), buffer.cdata().size());
                buffer.clear();
            }
        }
    } catch (boost::system::system_error & ex1) {
    } catch (std::exception& ex0) {
        log_warn(ex0.what());
    }

    if (isConnected && handler->on_disconnected != nullptr) {
        co_await handler->on_disconnected(client);
    }
    co_return;
}

int CoroWebSocketServer::start(short port,int hint) {
    std::condition_variable cv;
    std::mutex mtx;
    bool started = false;
    std::shared_ptr<boost::asio::io_context> ioc = std::make_shared<boost::asio::io_context>(hint);
    std::shared_ptr<std::thread> t = std::make_shared<std::thread>([&,port,ioc]() {
        try {
            co_spawn(*ioc,[&]() -> awaitable<void> {
                tcp::acceptor acceptor4(*ioc, tcp::endpoint(tcp::v4(), port));
                for (;;) {
                    tcp::socket socket = co_await acceptor4.async_accept(use_awaitable);
                    co_spawn(socket.get_executor(),this->session_handler(std::move(socket)),detached);
                }
            } , detached);
        } catch (std::exception& ex0) {
            log_info(ex0.what());
        }
        
        // co_spawn(*ioc,[&]() -> awaitable<void> {
        //     tcp::acceptor acceptor6(*ioc, tcp::endpoint(tcp::v6(), port));
        //     for (;;) {
        //         websocket::stream<tcp::socket> socket = co_await acceptor6.async_accept(use_awaitable);
        //         log_info("IPv6 接入一个新连接");
        //         co_spawn(socket.get_executor(), handle(std::move(socket),on_connected_handle,on_message_handle,on_disconnected_handle), detached);
        //     }
        // } , detached);
        
        log_info("Websocket Server [" << port << "] 已启动");
        started = true;
        cv.notify_one();
        ioc->run();
        log_info("Websocket Server [" << port << "] 已停止");
    });

    std::unique_lock<std::mutex> lock(mtx);
    log_info("Websocket Server [" << port << "] 正在启动");
    if (cv.wait_for(lock, std::chrono::seconds(3),[&]() { return started; })) {
        io_context = std::move(ioc);
        thread = std::move(t);
        return 0;
    } else {
        log_info("Websocket Server [" << port << "] 启动超时");
        ioc->stop();
        t->join();
        return -1;
    }
}

void CoroWebSocketServer::stop() {
    if (io_context != nullptr && thread != nullptr && thread->joinable()) {
        log_info("正在关闭");
        io_context->stop();
        thread->join();
    } else {
        log_error("未启动");
    }
}
void CoroWebSocketServer::add(std::string path, 
    std::function<awaitable<void>(websocket::stream<tcp::socket>&)> on_connected_handle,
    std::function<awaitable<void>(websocket::stream<tcp::socket>&,const char *,size_t)> on_message_handle,
    std::function<awaitable<void>(websocket::stream<tcp::socket>&)> on_disconnected_handle) {
        std::shared_ptr<CoroWebSocketHandler> handler = std::make_shared<CoroWebSocketHandler>(
            std::move(on_connected_handle),
            std::move(on_message_handle),
            std::move(on_disconnected_handle));
        routes.insert(path,std::move(handler)); 
}
CoroWebSocketServer CoroWebSocketServer::getTestInstance() {
    static CoroWebSocketServer websocket;
    websocket.add("/",[](websocket::stream<tcp::socket> &socket) -> awaitable<void> {
            log_info("已连接2");
            co_return;
        },
        [](websocket::stream<tcp::socket> &socket,const char * data,size_t size) -> awaitable<void> {
            co_await socket.async_write(boost::asio::buffer(data, size),use_awaitable);
            log_info("数据已发送2");
            co_return;
        },
        [](websocket::stream<tcp::socket> &socket) -> awaitable<void> {
            log_info("断连接2");
            co_return;
        }
    );

    websocket.add("/123",[](websocket::stream<tcp::socket> &socket) -> awaitable<void> {
            log_info("已连接1");
            co_return;
        },
        [](websocket::stream<tcp::socket> &socket,const char * data,size_t size) -> awaitable<void> {
            co_await socket.async_write(boost::asio::buffer(data, size),use_awaitable);
            log_info("数据已发送1");
            co_return;
        },
        [](websocket::stream<tcp::socket> &socket) -> awaitable<void> {
            log_info("断连接1");
            co_return;
        }
    );
    websocket.start(10082,1);
    return std::move(websocket);
}