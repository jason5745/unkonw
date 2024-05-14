#ifndef __BOOST_COROUTINE_TCP_SERVER_
#define __BOOST_COROUTINE_TCP_SERVER_

#include <iostream>
#include <thread>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace utils {
namespace boost {
namespace tcp {

using ::boost::asio::awaitable;
using ::boost::asio::co_spawn;
using ::boost::asio::detached;
using ::boost::asio::use_awaitable;
using ::boost::asio::ip::tcp;

class Server {
private:
    std::function<awaitable<void>(tcp::socket &)> on_connected_handle_;
    std::function<awaitable<void>(tcp::socket &, const char *, size_t)> on_message_handle_;
    std::function<awaitable<void>(tcp::socket &)> on_disconnected_handle_;

    std::shared_ptr<::boost::asio::io_context> io_context_;
    std::unique_ptr<std::thread> thread_;
public:
    Server(Server &&other) noexcept {
        on_connected_handle_ = std::move(other.on_connected_handle_);
        on_message_handle_ = std::move(other.on_message_handle_);
        on_disconnected_handle_ = std::move(other.on_disconnected_handle_);
        io_context_ = std::move(other.io_context_);
        thread_ = std::move(other.thread_);
    }
    Server &operator=(Server &&other) noexcept {
        if (this != &other) {
            on_connected_handle_ = std::move(other.on_connected_handle_);
            on_message_handle_ = std::move(other.on_message_handle_);
            on_disconnected_handle_ = std::move(other.on_disconnected_handle_);
            io_context_ = std::move(other.io_context_);
            thread_ = std::move(other.thread_);
        }
        return *this;
    }

    Server(std::function<awaitable<void>(tcp::socket &)> on_connected_handle,
           std::function<awaitable<void>(tcp::socket &, const char *, size_t)> on_message_handle,
           std::function<awaitable<void>(tcp::socket &)> on_disconnected_handle);

    ~Server();

    int start(short port, int hint);
    void stop();
    awaitable<void> session(tcp::socket socket);
    static Server &&getTestInstance();
};
}
}
}

#endif