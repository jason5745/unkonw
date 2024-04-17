

#include <condition_variable>
#include "coroutine_tcp_server.hpp"
#include "log.hpp"

awaitable<void> handle(tcp::socket socket,
    std::function<awaitable<void>(tcp::socket&)> on_connected_handle,
    std::function<awaitable<void>(tcp::socket&,const char *,size_t)> on_message_handle,
    std::function<awaitable<void>(tcp::socket&)> on_disconnected_handle) {
    char data[4096];
    if (on_connected_handle != nullptr) {
        co_await on_connected_handle(socket);
    }

    try {
        for (;;) {
            std::size_t n = co_await socket.async_read_some(boost::asio::buffer(data), use_awaitable);
            if (on_message_handle != nullptr) {
                co_await on_message_handle(socket, data, n);
            }
        }
    } catch (boost::system::system_error & ex1) {
    } catch (std::exception& ex0) {
        log_warn("" << ex0.what());
    }

    if (on_disconnected_handle != nullptr) {
        co_await on_disconnected_handle(socket);
    }
}

coroutine_tcp_server::coroutine_tcp_server(
    std::function<awaitable<void>(tcp::socket&)> on_connected_handle,
    std::function<awaitable<void>(tcp::socket&,const char *,size_t)> on_message_handle,
    std::function<awaitable<void>(tcp::socket&)> on_disconnected_handle) {

    this->on_connected_handle = on_connected_handle;
    this->on_message_handle = on_message_handle;
    this->on_disconnected_handle = on_disconnected_handle;
}
coroutine_tcp_server::~coroutine_tcp_server() {}

int coroutine_tcp_server::start(short port,int hint) {

    std::condition_variable cv;
    std::shared_ptr<boost::asio::io_context> ioc = std::make_shared<boost::asio::io_context>(hint);
    std::unique_ptr<std::thread> th = std::make_unique<std::thread>([&,port,ioc]() {

        co_spawn(*ioc,[&]() -> awaitable<void> {
            tcp::acceptor acceptor4(*ioc, tcp::endpoint(tcp::v4(), port));
            for (;;) {
                tcp::socket socket = co_await acceptor4.async_accept(use_awaitable);
                log_info("IPv4 接入一个新连接");
                co_spawn(socket.get_executor(), handle(std::move(socket),on_connected_handle,on_message_handle,on_disconnected_handle), detached);
            }
        } , detached);

        try {
            
        } catch (std::exception& ex0) {
            log_info(ex0.what());
        }
        
        // co_spawn(*ioc,[&]() -> awaitable<void> {
        //     tcp::acceptor acceptor6(*ioc, tcp::endpoint(tcp::v6(), port));
        //     for (;;) {
        //         tcp::socket socket = co_await acceptor6.async_accept(use_awaitable);
        //         log_info("IPv6 接入一个新连接");
        //         co_spawn(socket.get_executor(), handle(std::move(socket),on_connected_handle,on_message_handle,on_disconnected_handle), detached);
        //     }
        // } , detached);

        log_info("TCP Server [" + std::to_string(port) + "] 已启动");
        ioc->run();
        log_info("TCP Server [" + std::to_string(port) + "] 已停止");
    });

    std::mutex mtx;
    std::unique_lock<std::mutex> lock(mtx);
    if (!cv.wait_for(lock, std::chrono::milliseconds(5000),[]() { return false; })) {
        log_info("TCP Server [" + std::to_string(port) + "] 启动超时");
        ioc->stop();
        th->join();
        return -1;
    } else {
        io_context = std::move(ioc);
        thread = std::move(th);
        return 0;
    }
}

void coroutine_tcp_server::stop() {
    if (io_context != nullptr && thread != nullptr && thread->joinable()) {
        io_context->stop();
        thread->join();
    } else {
        log_error("服务未启动");
    }
}
void test() {
    coroutine_tcp_server server(
        [] (tcp::socket &socket) -> awaitable<void> {
            log_info(socket.remote_endpoint().address().to_string() 
                << ":" 
                << socket.remote_endpoint().port() 
                << " 已连接");
            co_return;
        },
        [](tcp::socket &socket,const char * data,size_t size) -> awaitable<void> {
            log_info(socket.remote_endpoint().address().to_string() 
                << ":" 
                << socket.remote_endpoint().port() 
                << " 收到数据");
            // co_await socket.async_write_some(boost::asio::buffer(data, size),use_awaitable);
            // log_info("数据已发送");
            co_return;
        },
        [](tcp::socket &socket) -> awaitable<void> {
            log_info(socket.remote_endpoint().address().to_string() 
                << ":" 
                << socket.remote_endpoint().port() 
                << " 已断开");
            co_return;
        }
    );
    server.start(10086,1);
    sleep(60 * 60);
    server.stop();
}