#ifndef __coro_TCP_SERVER_H_
#define __coro_TCP_SERVER_H_

#include <iostream>
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

class coro_tcp_server {
private:
    std::function<awaitable<void>(tcp::socket&)> on_connected_handle;
    std::function<awaitable<void>(tcp::socket&,const char *,size_t)> on_message_handle;
    std::function<awaitable<void>(tcp::socket&)> on_disconnected_handle;

    std::shared_ptr<boost::asio::io_context> io_context;
    std::unique_ptr<std::thread> thread;
public:
     coro_tcp_server(coro_tcp_server&& other) {
        on_connected_handle = std::move(other.on_connected_handle);
        on_message_handle = std::move(other.on_message_handle);
        on_disconnected_handle = std::move(other.on_disconnected_handle);
        io_context = std::move(other.io_context);
        thread = std::move(other.thread);
    }
    coro_tcp_server& operator=(coro_tcp_server&& other) {
        if (this != &other) {
            on_connected_handle = std::move(other.on_connected_handle);
            on_message_handle = std::move(other.on_message_handle);
            on_disconnected_handle = std::move(other.on_disconnected_handle);
            io_context = std::move(other.io_context);
            thread = std::move(other.thread);
        }
        return *this;
    }
    
    coro_tcp_server(
        std::function<awaitable<void>(tcp::socket&)> on_connected_handle,
        std::function<awaitable<void>(tcp::socket&,const char *,size_t)> on_message_handle,
        std::function<awaitable<void>(tcp::socket&)> on_disconnected_handle);

    ~coro_tcp_server();
    
    int start(short port,int hint);
    void stop();
    awaitable<void> session_handler(tcp::socket socket);
    static coro_tcp_server&& getTestInstance();
};
#endif