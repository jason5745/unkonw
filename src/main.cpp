
#include "boost_coro_httpd.h"
#include "boost_coro_tcpd.h"
#include "boost_coro_websocketd.h"
#include "uri_router.h"
#include "boost_log.h"
//#include "element/linked_list.h"

class A {
public:
    int value = 1;
    A(int a): value(a) {};
};

int main(int argc, char **argv) {
    logger::configure(boost::log::trivial::trace, 1);
    coro_http_server httpd = coro_http_server::getTestInstance();
    coro_tcp_server tcpd = coro_tcp_server::getTestInstance();
    coro_websocket_server wsd = coro_websocket_server::getTestInstance();
    //LinkedList<std::string> linked;



    uri_router<A> router;
    router.insert("/1/2/3/4",std::make_shared<A>(1));
    router.insert("/1/2",std::make_shared<A>(2));
    router.insert("/1/2/3/5",std::make_shared<A>(3));
    router.insert("//",std::make_shared<A>(4));

    
    std::cout << router.find("/1/2/3/4")->value << std::endl;
    std::cout << router.find("/1/2")->value << std::endl;
    std::cout << router.find("/1/2/3/5")->value << std::endl;
    std::cout << router.find("/")->value << std::endl;

    sleep(3600);
    tcpd.stop();
    httpd.stop();
    wsd.stop();
    return 0;
}
