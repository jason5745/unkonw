#ifndef __BOOST_COROUTINE_HTTP_SERVER_
#define __BOOST_COROUTINE_HTTP_SERVER_

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <unordered_map>
#include "uri_router.h"

namespace utils {
namespace boost {
namespace coroutine {
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

class Server {
private:
    bool keep_alive_;
    HttpFunction global_;
    UriRouter<HttpMethods> routes_;
    std::vector<Handler> handlers_;

public:
    Server(Server &&other) noexcept {
        handlers_ = std::move(other.handlers_);
        routes_ = std::move(other.routes_);
        global_ = std::move(other.global_);
        keep_alive_ = other.keep_alive_;
        other.keep_alive_ = 0;
    }

    Server &operator=(Server &&other) noexcept {
        if (this != &other) {
            handlers_ = std::move(other.handlers_);
            routes_ = std::move(other.routes_);
            global_ = std::move(other.global_);
            keep_alive_ = other.keep_alive_;
            other.keep_alive_ = 0;
        }
        return *this;
    }
    Server(size_t threads, bool keep_alive);
    Server(size_t threads, HttpFunction global, bool keep_alive);
    ~Server();
    int start(uint16_t port);
    void stop();
    void mount(verb method, std::string path, HttpFunction function);
    static Server &&getTestInstance();
    awaitable<void> session(int num, tcp::socket socket);
};
}
}
}
}
#endif