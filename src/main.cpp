
#include "coroutine_tcp_server.hpp"
#include "coroutine_http_server.hpp"
#include "log.hpp"


int main(int argc, char **argv) {
    logger::configure(boost::log::trivial::trace, 1);
    
    coroutine_http_server httpd = coroutine_http_server::getTestInstance();
    coroutine_tcp_server tcpd = coroutine_tcp_server::getTestInstance();

    
    sleep(3600);
    tcpd.stop();
    httpd.stop();

    return 0;
}
