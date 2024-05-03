#ifndef __BOOST_CORO_HTTPD_H_
#define __BOOST_CORO_HTTPD_H_

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <unordered_map>
#include "uri_router.h"

using boost::asio::ip::tcp;
using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;

namespace http = boost::beast::http;

typedef std::function<awaitable<http::response<http::string_body>>(tcp::socket& ,http::request<http::string_body>&)> HttpFunction;
typedef std::unordered_map<http::verb, HttpFunction> HttpMethods;

struct CoroHTTPSubServer{
    std::shared_ptr<boost::asio::io_context> io_context;
    std::unique_ptr<std::thread> thread;
};

class CoroHTTPServer {
private:
    bool _keepAlive;
    UriRouter<HttpMethods> _routes;
    std::vector<CoroHTTPSubServer> _subservices;

public:
    CoroHTTPServer(CoroHTTPServer&& other) {
        _subservices = std::move(other._subservices);
        _routes = std::move(other._routes);
        _keepAlive = other._keepAlive;
        other._keepAlive = 0;
    }
    CoroHTTPServer& operator=(CoroHTTPServer&& other) {
        if (this != &other) {
            _subservices = std::move(other._subservices);
            _routes = std::move(other._routes);
            _keepAlive = other._keepAlive;
            other._keepAlive = 0;
        }
        return *this;
    }

    CoroHTTPServer(size_t threads,bool keepAlive);
    ~CoroHTTPServer();

    int start(uint16_t port);
    void stop();
    awaitable<void> session_handler(tcp::socket socket);
    void add(http::verb method, std::string path, HttpFunction function);

    static CoroHTTPServer&& getTestInstance();
};
#endif