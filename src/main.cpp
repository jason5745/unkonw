
#include "coro_http_server.h"
#include "coro_tcp_server.h"
#include "epoll_tcp_server.h"
#include "coro_websocket_server.h"
#include "uri_router.h"
#include "logger.h"
#include "element/ringbuf.h"

//#include "element/linked_list.h"

class A {
public:
    int value = 1;
    A() {value = 1;};
    A(int a): value(a) {};
};

int main(int argc, char **argv) {
    // Logger::configure(boost::log::trivial::trace, 1);
    
    // CoroHTTPServer coro_http_server = CoroHTTPServer::getTestInstance();
    // CoroTCPServer coro_tcp_server = CoroTCPServer::getTestInstance();
    // CoroWebSocketServer coro_websocket_server = CoroWebSocketServer::getTestInstance();
    // EpollTCPServer epoll_tcp_server = EpollTCPServer::getTestInstance();

    // sleep(20);

    // coro_tcp_server.stop();
    // coro_http_server.stop();
    // coro_websocket_server.stop();
    // epoll_tcp_server.stop();
    ringbuf<A> buf(100);
    std::thread t1([&]{
        A a = A();
        while (true)
        {
            if (buf.pop(a)) {
                log_info("pop : " << a.value);
            }
            usleep(1000000);
        }
        
    });

    int i = 0;
    
    while (true)
    {
        
        A a(i++);
        if (buf.push(std::move(a))) {
            log_info("push: " << a.value);
        }
        usleep(1000);
    }
    
    return 0;
}

