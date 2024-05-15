#ifndef __BOOST_ASYNC_HTTP_SERVER_
#define __BOOST_ASYNC_HTTP_SERVER_

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <unordered_map>
#include "uri_router.h"

namespace utils {
namespace boost {
namespace async {
namespace http {

using ::boost::asio::ip::tcp;
using ::boost::asio::awaitable;
using ::boost::asio::co_spawn;
using ::boost::asio::detached;
using ::boost::asio::use_awaitable;
using namespace ::boost::beast::http;

typedef std::function<awaitable<response<string_body>>(int num, tcp::socket &, request<string_body> &)> HttpFunction;
typedef std::unordered_map<verb, HttpFunction> HttpMethods;

struct Handler {
    std::shared_ptr<::boost::asio::io_context> io_context;
    std::unique_ptr<std::thread> thread;
};

class Session : public std::enable_shared_from_this<Session> {
private:
    int num_;
    std::function<http::message_generator(int, request<string_body> &&)> request_handler_;
    ::boost::beast::tcp_stream stream_;
    ::boost::beast::flat_buffer buffer_;
    ::boost::beast::http::request<http::string_body> req_;
public:
    Session(int num,
            std::function<http::message_generator(int, request<string_body> &&)> handler,
            tcp::socket &&socket);
    ~Session();

    void run();
    void read();
    void onRequest(::boost::beast::error_code ec, std::size_t bytes_transferred);
    void onResponse(bool keep_alive, ::boost::beast::error_code ec, std::size_t bytes_transferred);

};
class Server {
private:
    UriRouter<HttpMethods> routes_;
    std::vector<Handler> handlers_;
    std::function<http::message_generator(int, request<string_body> &&)> request_handler_;
public:
    Server(Server &&other) noexcept {
        handlers_ = std::move(other.handlers_);
        routes_ = std::move(other.routes_);
        request_handler_ = std::move(other.request_handler_);
    }

    Server &operator=(Server &&other) noexcept {
        if (this != &other) {
            handlers_ = std::move(other.handlers_);
            routes_ = std::move(other.routes_);
            request_handler_ = std::move(other.request_handler_);
        }
        return *this;
    }
    Server(size_t threads);
    Server(size_t threads, std::function<http::message_generator(int, request<string_body> &&)> request_handler);
    ~Server();
    int start(uint16_t port);
    void stop();
    void mount(verb method, std::string path, HttpFunction function);
    void accept(int num,
                const std::shared_ptr<tcp::acceptor> &acceptor,
                const std::shared_ptr<::boost::asio::io_context> &io_context);
    http::message_generator static handler(int num, request<string_body> &&req);
    static Server &&getTestInstance();
};
}
}
}
}
#endif