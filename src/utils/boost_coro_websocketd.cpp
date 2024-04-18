


#include <condition_variable>
#include <boost/url/url.hpp>
#include "boost_coro_websocketd.h"
#include "boost_log.h"

using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;
namespace websocket = boost::beast::websocket;
namespace http = boost::beast::http;

static boost::beast::flat_buffer buffer;
awaitable<void> coro_websocket_server::session_handler(tcp::socket socket) {
    bool isConnected = false;
    http::request<http::string_body> request;
    try {   //判断是否为Websocket握手请求
        co_await http::async_read(socket, buffer, request, use_awaitable);
        if (!websocket::is_upgrade(request)) co_return;
            //TODO: check websocket
            boost::urls::url url(request.target());
            
    } catch (std::exception& ex0) {
        log_warn(ex0.what());
        co_return;
    }

    websocket::stream<tcp::socket> client(std::move(socket));
    try {
        log_info(request.target());
        co_await client.async_accept(request, use_awaitable);
        
        isConnected = true;
        if (on_connected_handle != nullptr) {
            co_await on_connected_handle(client);
        }
        for (;;) {
            std::size_t n = co_await client.async_read(buffer, use_awaitable);
            if (on_message_handle != nullptr) {
                co_await on_message_handle(client, (char *)buffer.cdata().data(), buffer.cdata().size());
            }
        }
    } catch (boost::system::system_error & ex1) {
    } catch (std::exception& ex0) {
        log_warn(ex0.what());
    }
    if (isConnected && on_disconnected_handle != nullptr) {
        co_await on_disconnected_handle(client);
    }
    co_return;
}

coro_websocket_server::coro_websocket_server(
    std::function<awaitable<void>(websocket::stream<tcp::socket>&)> on_connected_handle,
    std::function<awaitable<void>(websocket::stream<tcp::socket>&,const char *,size_t)> on_message_handle,
    std::function<awaitable<void>(websocket::stream<tcp::socket>&)> on_disconnected_handle) {

    this->on_connected_handle = on_connected_handle;
    this->on_message_handle = on_message_handle;
    this->on_disconnected_handle = on_disconnected_handle;
}

coro_websocket_server::~coro_websocket_server() {}

int coro_websocket_server::start(short port,int hint) {

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


        
        log_info("TCP Server [" + std::to_string(port) + "] 已启动");
        started = true;
        cv.notify_one();
        ioc->run();
        log_info("TCP Server [" + std::to_string(port) + "] 已停止");
    });

    std::unique_lock<std::mutex> lock(mtx);
    if (cv.wait_for(lock, std::chrono::seconds(3),[&]() { return started; })) {
        log_info("TCP Server [" + std::to_string(port) + "] 启动成功");
        io_context = std::move(ioc);
        thread = std::move(t);
        return 0;
    } else {
        log_info("TCP Server [" + std::to_string(port) + "] 启动超时");
        ioc->stop();
        t->join();
        return -1;
    }
}

void coro_websocket_server::stop() {
    log_info("正在关闭服务");
    if (io_context != nullptr && thread != nullptr && thread->joinable()) {
        io_context->stop();
        thread->join();
        log_info("已关闭服务");
    } else {
        log_error("服务未启动");
    }
}

coro_websocket_server coro_websocket_server::getTestInstance() {
    static coro_websocket_server wsd(
        [](websocket::stream<tcp::socket> &socket) -> awaitable<void> {
            
            // log_info(socket.remote_endpoint().address().to_string() 
            //     << ":" 
            //     << socket.remote_endpoint().port() 
            //     << " 已连接");
            co_return;
        },
        [](websocket::stream<tcp::socket> &socket,const char * data,size_t size) -> awaitable<void> {
            // log_info(socket.remote_endpoint().address().to_string()
            //     << ":"
            //     << socket.remote_endpoint().port() 
            //     << " 收到数据: " << ByteToHexString(data,size));
            //socket.
            co_await socket.async_write(boost::asio::buffer(data, size),use_awaitable);
            log_info("数据已发送");
            co_return;
        },
        [](websocket::stream<tcp::socket> &socket) -> awaitable<void> {
            // log_info(socket.remote_endpoint().address().to_string() 
            //     << ":" 
            //     << socket.remote_endpoint().port() 
            //     << " 已断开");
            co_return;
        }
    );
    wsd.start(10082,1);
    return std::move(wsd);
}