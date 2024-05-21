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

typedef std::function<awaitable<response<string_body>>(::std::size_t tid, tcp::socket &, request<string_body> &)> HttpFunction;
typedef std::unordered_map<verb, HttpFunction> HttpMethods;

class WorkHandler {
private:
    ::boost::asio::io_context ctx;
    ::std::thread th;
    ::std::size_t tid;
    ::std::size_t members = 0;
public:
    WorkHandler() = default;
    ~WorkHandler() = default;

    WorkHandler(const WorkHandler&) = delete;
    WorkHandler &operator=(const WorkHandler&) = delete;
    WorkHandler(WorkHandler &&) = delete;
    WorkHandler &operator=(WorkHandler &&) = delete;

    friend class Server;
};

class Server {
private:
	HttpFunction global_;
	UriRouter<HttpMethods> routes_;
    std::vector<WorkHandler> workers_;

public:
	Server(Server &&other) noexcept {
        workers_ = std::move(other.workers_);

		routes_ = std::move(other.routes_);
		global_ = std::move(other.global_);
	}

	Server &operator=(Server &&other) noexcept {
		if (this != &other) {
            workers_ = std::move(other.workers_);
			routes_ = std::move(other.routes_);
			global_ = std::move(other.global_);
		}
		return *this;
	}
	Server(size_t threads);
	Server(size_t threads, HttpFunction global);
	~Server();
	int start(uint16_t port);
	void stop();
	void mount(verb method, std::string path, HttpFunction function);
	static Server &&getTestInstance();
    awaitable<void> session(WorkHandler *worker, tcp::socket socket);
};
}
}
}
}
#endif