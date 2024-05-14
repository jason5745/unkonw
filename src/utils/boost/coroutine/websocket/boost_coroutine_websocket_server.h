#ifndef __BOOST_COROUTINE_WEBSOCKET_SERVER_
#define __BOOST_COROUTINE_WEBSOCKET_SERVER_

#include <thread>
#include <boost/asio.hpp>
#include <utility>
#include <boost/beast/websocket.hpp>
#include "uri_router.h"

namespace utils {
namespace boost {
namespace websocket {

using ::boost::asio::ip::tcp;
using ::boost::asio::co_spawn;
using ::boost::asio::detached;
using ::boost::asio::awaitable;
using ::boost::asio::use_awaitable;

using ::boost::beast::websocket::stream;

class Handler {
private:
    std::function<awaitable<void>(stream<tcp::socket> &)> on_connected_;
    std::function<awaitable<void>(stream<tcp::socket> &, const char *, size_t)> on_message_;
    std::function<awaitable<void>(stream<tcp::socket> &)> on_disconnected_;
public:
    Handler(
        std::function<awaitable<void>(stream<tcp::socket> &)> on_connected_handle,
        std::function<awaitable<void>(stream<tcp::socket> &, const char *, size_t)> on_message_handle,
        std::function<awaitable<void>(stream<tcp::socket> &)> on_disconnected_handle) :
        on_connected_(std::move(on_connected_handle)),
        on_message_(std::move(on_message_handle)),
        on_disconnected_(std::move(on_disconnected_handle)) {};
    ~Handler() {};
    friend class Server;
};

class Server {
private:
    std::shared_ptr<::boost::asio::io_context> io_context_;
    std::shared_ptr<std::thread> thread_;
    UriRouter<Handler> routes_;
public:
    Server() {};
    Server(Server &&other) noexcept {
        routes_ = other.routes_;
        io_context_ = std::move(other.io_context_);
        thread_ = std::move(other.thread_);
    }
    Server &operator=(Server &&other) noexcept {
        if (this != &other) {
            routes_ = other.routes_;
            io_context_ = std::move(other.io_context_);
            thread_ = std::move(other.thread_);
        }
        return *this;
    }
    ~Server() {};
    int start(short port, int hint);
    void stop();
    void mount(
        std::string path,
        std::function<awaitable<void>(websocket::stream<tcp::socket> &)> on_connected_handle,
        std::function<awaitable<void>(websocket::stream<tcp::socket> &, const char *, size_t)> on_message_handle,
        std::function<awaitable<void>(websocket::stream<tcp::socket> &)> on_disconnected_handle);
    awaitable<void> session(tcp::socket socket);
    static Server getTestInstance();
};
}
}
}
#endif