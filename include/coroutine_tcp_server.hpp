#ifndef __COROUTINE_TCP_SERVER_H_
#define __COROUTINE_TCP_SERVER_H_

#include <thread>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/write.hpp>

using boost::asio::ip::tcp;
using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;

class coroutine_tcp_server {
private:
    std::function<awaitable<void>(tcp::socket&)> on_connected_handle = nullptr;
    std::function<awaitable<void>(tcp::socket&,const char *,size_t)> on_message_handle  = nullptr;
    std::function<awaitable<void>(tcp::socket&)> on_disconnected_handle = nullptr;

    std::shared_ptr<boost::asio::io_context> io_context;
    std::unique_ptr<std::thread> thread;
public:
    coroutine_tcp_server(
        std::function<awaitable<void>(tcp::socket&)> on_connected_handle,
        std::function<awaitable<void>(tcp::socket&,const char *,size_t)> on_message_handle,
        std::function<awaitable<void>(tcp::socket&)> on_disconnected_handle);
    ~coroutine_tcp_server();
    void set_io_context(boost::asio::io_context *io_context);
    int start(short port,int hint);
    void stop();
};
#endif