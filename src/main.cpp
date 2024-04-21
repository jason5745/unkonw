
#include "coro_http_server.h"
#include "coro_tcp_server.h"
#include "coro_websocket_server.h"
#include "uri_router.h"
#include "logger.h"

//#include "element/linked_list.h"

class A {
public:
    int value = 1;
    A(int a): value(a) {};
};

int main(int argc, char **argv) {
    Logger::configure(boost::log::trivial::trace, 1);
    
    CoroHTTPServer httpd = CoroHTTPServer::getTestInstance();
    CoroTCPServer tcpd = CoroTCPServer::getTestInstance();
    CoroWebSocketServer wsd = CoroWebSocketServer::getTestInstance();
    
    sleep(3600);
    tcpd.stop();
    httpd.stop();
    wsd.stop();
    return 0;
}
