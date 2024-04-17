#ifndef __COROUTINE_HTTP_SERVER_H_
#define __COROUTINE_HTTP_SERVER_H_

#include <iostream>
#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/spawn.hpp>
#include <unordered_map>

using boost::asio::ip::tcp;
using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;

namespace http = boost::beast::http;

typedef std::function<awaitable<http::response<http::string_body>>(tcp::socket& ,http::request<http::string_body>&)> HttpFunction;
typedef std::unordered_map<http::verb, HttpFunction> HttpMethods;
typedef std::unordered_map<std::string, HttpMethods> HttpRoute;

class coroutine_http_server {
private:
    bool keepAlive;
    HttpRoute routes;
    std::shared_ptr<boost::asio::io_context> io_context;
    std::shared_ptr<std::thread> thread;

public:
    coroutine_http_server(bool isKeepAlive): keepAlive(isKeepAlive) {};
    ~coroutine_http_server() {};

    int start(short port,int hint);
    void stop();
    awaitable<void> session_handler(tcp::socket socket);
    void add(http::verb method, std::string path, HttpFunction function);

    static coroutine_http_server getTestInstance();
};
#endif