

#include <condition_variable>
#include <boost/url/url.hpp>
#include "boost_coroutine_websocket_server.h"
#include "logger.h"

namespace utils {
namespace boost {
namespace websocket {

using namespace ::boost::beast::http;
using namespace ::boost::beast::websocket;

awaitable<void> Server::session(tcp::socket socket) {
    ::boost::beast::flat_buffer buffer;
    bool is_connected = false;
    request<string_body> request;
    std::shared_ptr<Handler> handler = nullptr;
    try {
        (void) co_await async_read(socket, buffer, request, use_awaitable);
        ::boost::urls::url url(request.target());
        handler = this->routes_.find(url.path());
        if (handler == nullptr) {   //判断是否已注册过路由
            response<string_body> resp = response<string_body>{status::not_found, request.version()};
            (void) co_await async_write(socket, resp, use_awaitable);
            co_return;
        }
        if (!is_upgrade(request)) {  //判断是否为Websocket握手请求
            response<string_body> resp = response<string_body>{status::not_modified, request.version()};
            (void) co_await async_write(socket, resp, use_awaitable);
            co_return;
        }
        // TODO: Auth
    } catch (std::exception &ex0) {
        log_warn(ex0.what());
        co_return;
    }
    //开始Websocket会话
    websocket::stream<tcp::socket> client(std::move(socket));
    try {
        (void) co_await client.async_accept(request, use_awaitable);
        is_connected = true;
        if (handler->on_connected_ != nullptr) {
            (void) co_await handler->on_connected_(client);
        }
        for (;;) {
            (void) co_await client.async_read(buffer, use_awaitable);
            if (handler->on_message_ != nullptr) {
                (void) co_await handler->on_message_(client,
                                                     static_cast<const char *>(buffer.cdata().data()),
                                                     buffer.cdata().size());
                buffer.clear();
            }
        }
    } catch (::boost::system::system_error &ex1) {
    } catch (std::exception &ex0) {
        log_warn(ex0.what());
    }

    buffer.clear();
    if (is_connected && handler->on_disconnected_ != nullptr) {
        (void) co_await handler->on_disconnected_(client);
    }
    co_return;
}

int Server::start(short port, int hint) {
    std::condition_variable cv;
    std::mutex mtx;
    bool started = false;
    std::shared_ptr<::boost::asio::io_context> ioc = std::make_shared<::boost::asio::io_context>(hint);
    std::shared_ptr<std::thread> t = std::make_shared<std::thread>([&, port, ioc]() {
        try {
            (void) co_spawn(*ioc, [&]() -> awaitable<void> {
                tcp::acceptor acceptor4(*ioc, tcp::endpoint(tcp::v4(), port));
                for (;;) {
                    tcp::socket socket = co_await acceptor4.async_accept(use_awaitable);
                    (void) co_spawn(ioc->get_executor(), this->session(std::move(socket)), detached);
                }
            }, detached);
        } catch (std::exception &ex0) {
            log_info(ex0.what());
        }

        log_info("Websocket Server [{}] 已启动", port);
        started = true;
        cv.notify_one();
        (void) ioc->run();
        log_info("Websocket Server [{}] 已停止", port);
    });

    std::unique_lock<std::mutex> lock(mtx);
    log_info("Websocket Server [{}] 正在启动", port);
    if (cv.wait_for(lock, std::chrono::seconds(3L), [&]() { return started; })) {
        io_context_ = std::move(ioc);
        thread_ = std::move(t);
        return 0;
    } else {
        log_info("Websocket Server [{}] 启动超时", port);
        ioc->stop();
        t->join();
        return -1;
    }
}

void Server::stop() {
    if (io_context_ != nullptr && thread_ != nullptr && thread_->joinable()) {
        log_info("正在关闭");
        io_context_->stop();
        thread_->join();
    } else {
        log_error("未启动");
    }
}
void Server::mount(
    std::string path,
    std::function<awaitable<void>(websocket::stream<tcp::socket> &)> on_connected_handle,
    std::function<awaitable<void>(websocket::stream<tcp::socket> &, const char *, size_t)> on_message_handle,
    std::function<awaitable<void>(websocket::stream<tcp::socket> &)> on_disconnected_handle) {
    std::shared_ptr<Handler> handler = std::make_shared<Handler>(
        std::move(on_connected_handle),
        std::move(on_message_handle),
        std::move(on_disconnected_handle));
    routes_.insert(std::move(path), std::move(handler));
}
Server Server::getTestInstance() {
    static Server websocket;
    websocket.mount("/", [](websocket::stream<tcp::socket> &socket) -> awaitable<void> {
                        log_info("已连接2");
                        co_return;
                    },
                    [](websocket::stream<tcp::socket> &socket, const char *data, size_t size) -> awaitable<void> {
                        (void) co_await socket.async_write(::boost::asio::buffer(data, size), use_awaitable);
                        log_info("数据已发送2");
                        co_return;
                    },
                    [](websocket::stream<tcp::socket> &socket) -> awaitable<void> {
                        log_info("断连接2");
                        co_return;
                    }
    );

    websocket.mount("/123", [](websocket::stream<tcp::socket> &socket) -> awaitable<void> {
                        log_info("已连接1");
                        co_return;
                    },
                    [](websocket::stream<tcp::socket> &socket, const char *data, size_t size) -> awaitable<void> {
                        (void) co_await socket.async_write(::boost::asio::buffer(data, size), use_awaitable);
                        log_info("数据已发送1");
                        co_return;
                    },
                    [](websocket::stream<tcp::socket> &socket) -> awaitable<void> {
                        log_info("断连接1");
                        co_return;
                    }
    );
    return std::move(websocket);
}
}
}
}