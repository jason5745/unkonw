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

class CoroHTTPServer {
private:
    bool keepAlive;
    UriRouter<HttpMethods> routes;
    std::shared_ptr<boost::asio::io_context> io_context;
    std::unique_ptr<std::thread> thread;

public:
    CoroHTTPServer(CoroHTTPServer&& other) {
        io_context = std::move(other.io_context);
        routes = std::move(other.routes);
        thread = std::move(other.thread);
        keepAlive = other.keepAlive;
        other.keepAlive = 0;
    }
    CoroHTTPServer& operator=(CoroHTTPServer&& other) {
        if (this != &other) {
            io_context = std::move(other.io_context);
            routes = std::move(other.routes);
            thread = std::move(other.thread);
            keepAlive = other.keepAlive;
            other.keepAlive = 0;
        }
        return *this;
    }

    CoroHTTPServer(bool isKeepAlive): keepAlive(isKeepAlive) {};
    ~CoroHTTPServer() {};

    int start(short port,int hint);
    void stop();
    awaitable<void> session_handler(tcp::socket socket);
    void add(http::verb method, std::string path, HttpFunction function);

    static CoroHTTPServer&& getTestInstance();
};
#endif