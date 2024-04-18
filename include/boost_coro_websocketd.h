#ifndef __BOOST_CORO_WEBSOCKETD_H_
#define __BOOST_CORO_WEBSOCKETD_H_

#include <thread>
#include <boost/asio.hpp>
#include <boost/beast/websocket.hpp>

using boost::asio::ip::tcp;
using boost::asio::awaitable;
namespace websocket = boost::beast::websocket;

class coro_websocket_server {
private:
    std::function<awaitable<void>(websocket::stream<tcp::socket>&)> on_connected_handle;
    std::function<awaitable<void>(websocket::stream<tcp::socket>&,const char *,size_t)> on_message_handle;
    std::function<awaitable<void>(websocket::stream<tcp::socket>&)> on_disconnected_handle;

    std::shared_ptr<boost::asio::io_context> io_context;
    std::shared_ptr<std::thread> thread;
public:

    coro_websocket_server(
        std::function<awaitable<void>(websocket::stream<tcp::socket>&)> on_connected_handle,
        std::function<awaitable<void>(websocket::stream<tcp::socket>&,const char *,size_t)> on_message_handle,
        std::function<awaitable<void>(websocket::stream<tcp::socket>&)> on_disconnected_handle);

    ~coro_websocket_server();
    
    int start(short port,int hint);
    void stop();
    awaitable<void> session_handler(tcp::socket socket);
    static coro_websocket_server getTestInstance();
};
#endif