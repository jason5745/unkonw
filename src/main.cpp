
#include "coro_tcp_server.hpp"
#include "coro_http_server.hpp"
#include "coro_websocket_server.hpp"
#include "log.hpp"

int main(int argc, char **argv) {
    logger::configure(boost::log::trivial::trace, 1);
    coro_http_server httpd = coro_http_server::getTestInstance();
    coro_tcp_server tcpd = coro_tcp_server::getTestInstance();
    coro_websocket_server wsd = coro_websocket_server::getTestInstance();
    
    sleep(3600);
    tcpd.stop();
    httpd.stop();
    wsd.stop();
    return 0;
}
