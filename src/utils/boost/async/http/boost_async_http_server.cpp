
#include <thread>
#include <functional>
#include <netinet/in.h>
#include <boost/url/url.hpp>
#include "logger.h"
#include "boost_async_http_server.h"


#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

namespace utils {
namespace boost {
namespace async {
namespace http {

Session::Session(int num,
                 std::function<http::message_generator(int, request<string_body> &&)> handler,
                 tcp::socket &&socket) : num_(num), request_handler_(std::move(handler)), stream_(std::move(socket)) {
    buffer_.reserve(8196);
//    log_info("[{}]Session Create",num_);
}
Session::~Session() {
//    log_info("[{}]Session Delete",num_);
}
void Session::run() {
    (void) ::boost::asio::dispatch(stream_.get_executor(),
                                   ::boost::beast::bind_front_handler(&Session::read, shared_from_this()));
}
void Session::read()  {
    req_ = {};
    buffer_.clear();
    //stream_.expires_after(std::chrono::seconds(30));
    (void) http::async_read(stream_, buffer_, req_,
                     ::boost::beast::bind_front_handler(&Session::onRequest, shared_from_this()));
}
void Session::onRequest(::boost::beast::error_code ec, std::size_t bytes_transferred)  {
    if (ec == http::error::end_of_stream) {
        ::boost::beast::error_code e;
        stream_.socket().shutdown(tcp::socket::shutdown_send, e);
        return;
    }
    if (unlikely(ec)) {
        log_info("Receive Error: {}", ec.what());
        return;
    }

    if (unlikely(!request_handler_)) {
        stream_.socket().shutdown(tcp::socket::shutdown_send, ec);
    } else {
        bool keep_alive = req_.keep_alive();
        (void) ::boost::beast::async_write(stream_,
            std::move(request_handler_(num_,std::move(req_))),
            ::boost::beast::bind_front_handler(&Session::onResponse, shared_from_this(), keep_alive));
    }
}
void Session::onResponse(bool keep_alive, ::boost::beast::error_code ec, std::size_t bytes_transferred) {
    if(unlikely(ec)) {
        log_info("Dispatch Error: {}", ec.what());
        return;
    }
    if (!keep_alive) {
        stream_.socket().shutdown(tcp::socket::shutdown_send, ec);
        return;
    }
    read();
}

http::message_generator Server::handler(int num, request<string_body> &&req) {
    http::response<http::string_body> res{http::status::ok, req.version()};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "application/json");
    res.keep_alive(req.keep_alive());
    res.body() = "{\"code\": 0}";
    res.prepare_payload();
    return std::move(res);
}

Server::Server(size_t threads) {
    request_handler_ = &Server::handler;
    for (size_t i = 0; i < threads; i++) {
        (void) handlers_.emplace_back();
    }
}

Server::Server(size_t threads,std::function<http::message_generator(int, request<string_body> &&)> handler)
    : request_handler_(std::move(handler)) {
    for (size_t i = 0; i < threads; i++) {
        (void) handlers_.emplace_back();
    }
}

Server::~Server() = default;

int Server::start(uint16_t port) {
    int thread_num = 0;
    for (auto &handler : handlers_) {
        handler.io_context = std::make_shared<::boost::asio::io_context>();
        handler.thread = std::make_unique<std::thread>([io_context = handler.io_context, port, this, thread_num]() {
            std::shared_ptr<tcp::acceptor> acceptor = std::make_shared<tcp::acceptor>(*io_context);
            acceptor->open(tcp::v4());
            int on = 1;
            (void) setsockopt(acceptor->native_handle(), SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on));
            struct sockaddr_in sa {
                .sin_family = AF_INET,
                .sin_port = htons(port),
                .sin_addr {
                    .s_addr = INADDR_ANY
                }
            };
            (void) bind(acceptor->native_handle(), (const sockaddr *) &sa, sizeof(sa));
            // acceptor.set_option(tcp::acceptor::reuse_address(true));
            // acceptor.bind(tcp::endpoint(tcp::v4(), port));
            acceptor->listen();
            accept(thread_num,acceptor,io_context);

            io_context->run();
        });
        thread_num++;
    }
    return 0;
}

void Server::accept(int num,const std::shared_ptr<tcp::acceptor>& acceptor,const std::shared_ptr<::boost::asio::io_context>& io_context) {
    (void)acceptor->async_accept(
        ::boost::asio::make_strand(*io_context),
        [&,num,acceptor,io_context] (::boost::beast::error_code ec,tcp::socket socket) -> void {
        if (unlikely(ec)) {
            log_error("Server Accept Error: {}", ec.what());
        } else {
            std::make_shared<Session>(num,request_handler_,std::move(socket))->run();
        }
        accept(num,acceptor,io_context);
    });
}

void Server::stop() {
    for (auto &handler : handlers_) {
        if (handler.io_context != nullptr && handler.thread != nullptr && handler.thread->joinable()) {
            log_info("正在关闭");
            handler.io_context->stop();
            handler.thread->join();
        } else {
            log_error("未启动");
        }
    }
}

void Server::mount(http::verb method, std::string path, HttpFunction function) {
    auto methods = this->routes_.find(path);
    if (methods == nullptr) {
        std::shared_ptr<HttpMethods> methods = std::make_shared<HttpMethods>();
        methods->insert({method, std::move(function)});
        routes_.insert(path, std::move(methods));
    } else {
        auto item = methods->find(method);
        if (item == methods->end()) {
            methods->insert({method, function});
        } else {
            log_warn("{}", "已添加路由");
        }
    }
}

Server &&Server::getTestInstance() {
    static Server service(4);
    return std::move(service);
}
}
}
}
}