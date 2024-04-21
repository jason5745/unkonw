#ifndef __BOOST_CORO_WEBSOCKETD_H_
#define __BOOST_CORO_WEBSOCKETD_H_

#include <thread>
#include <boost/asio.hpp>
#include <boost/beast/websocket.hpp>
#include <uri_router.h>

using boost::asio::ip::tcp;
using boost::asio::awaitable;
namespace websocket = boost::beast::websocket;

class CoroWebSocketHandler {
public:
    std::function<awaitable<void>(websocket::stream<tcp::socket>&)> on_connected;
    std::function<awaitable<void>(websocket::stream<tcp::socket>&,const char *,size_t)> on_message;
    std::function<awaitable<void>(websocket::stream<tcp::socket>&)> on_disconnected;

    CoroWebSocketHandler(std::function<awaitable<void>(websocket::stream<tcp::socket>&)> on_connected_handle,
    std::function<awaitable<void>(websocket::stream<tcp::socket>&,const char *,size_t)> on_message_handle,
    std::function<awaitable<void>(websocket::stream<tcp::socket>&)> on_disconnected_handle): 
    on_connected(on_connected_handle), on_message(on_message_handle), on_disconnected(on_disconnected_handle) {};
    ~CoroWebSocketHandler() {};
};

class CoroWebSocketServer {
private:
    std::shared_ptr<boost::asio::io_context> io_context;
    std::shared_ptr<std::thread> thread;
public:
    UriRouter<CoroWebSocketHandler> routes;

    CoroWebSocketServer() {};
    CoroWebSocketServer(CoroWebSocketServer&& other) {
        routes = std::move(other.routes);
        io_context = std::move(other.io_context);
        thread = std::move(other.thread);
    }
    CoroWebSocketServer& operator=(CoroWebSocketServer&& other) {
        if (this != &other) {
            routes = std::move(other.routes);
            io_context = std::move(other.io_context);
            thread = std::move(other.thread);
        }
        return *this;
    }
    ~CoroWebSocketServer() {};

    int start(short port,int hint);
    void stop();
    void add(std::string path, std::function<awaitable<void>(websocket::stream<tcp::socket>&)> on_connected_handle,
        std::function<awaitable<void>(websocket::stream<tcp::socket>&,const char *,size_t)> on_message_handle,
        std::function<awaitable<void>(websocket::stream<tcp::socket>&)> on_disconnected_handle);
    awaitable<void> session_handler(tcp::socket socket);
    static CoroWebSocketServer getTestInstance();
};
#endif