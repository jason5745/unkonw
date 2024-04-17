
#include <thread>

#include "coroutine_http_server.hpp"
#include "log.hpp"

using namespace boost::beast;

string_view mime_type(string_view path) {
    using boost::beast::iequals;
    auto const ext = [&path]
    {
        auto const pos = path.rfind(".");
        if(pos == string_view::npos)
            return string_view{};
        return path.substr(pos);
    } ();
    if(iequals(ext, ".htm"))  return "text/html";
    if(iequals(ext, ".html")) return "text/html";
    if(iequals(ext, ".php"))  return "text/html";
    if(iequals(ext, ".css"))  return "text/css";
    if(iequals(ext, ".txt"))  return "text/plain";
    if(iequals(ext, ".js"))   return "application/javascript";
    if(iequals(ext, ".json")) return "application/json";
    if(iequals(ext, ".xml"))  return "application/xml";
    if(iequals(ext, ".swf"))  return "application/x-shockwave-flash";
    if(iequals(ext, ".flv"))  return "video/x-flv";
    if(iequals(ext, ".png"))  return "image/png";
    if(iequals(ext, ".jpe"))  return "image/jpeg";
    if(iequals(ext, ".jpeg")) return "image/jpeg";
    if(iequals(ext, ".jpg"))  return "image/jpeg";
    if(iequals(ext, ".gif"))  return "image/gif";
    if(iequals(ext, ".bmp"))  return "image/bmp";
    if(iequals(ext, ".ico"))  return "image/vnd.microsoft.icon";
    if(iequals(ext, ".tiff")) return "image/tiff";
    if(iequals(ext, ".tif"))  return "image/tiff";
    if(iequals(ext, ".svg"))  return "image/svg+xml";
    if(iequals(ext, ".svgz")) return "image/svg+xml";
    return "application/text";
}
static std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    size_t start = 0, end = 0;
    while ((end = str.find(delimiter, start)) != std::string::npos) {
        tokens.push_back(str.substr(start, end - start));
        start = end + 1;
    }
    tokens.push_back(str.substr(start));
    return tokens;
}
awaitable<void> coroutine_http_server::session_handler(tcp::socket socket) {
    flat_buffer buffer;
    try {
        for (;;) {
            http::request<http::string_body> request;
            co_await http::async_read(socket, buffer, request, use_awaitable);
            auto tokens = split(request.target(),'?');
            if (tokens.size() > 0) {
                auto methods = routes.find(tokens[0]);
                if (methods != routes.end()) {
                    auto function = methods->second.find(request.method());
                    if (function != methods->second.end()) {
                        http::response<http::string_body> response = co_await function->second(socket,request);
                        co_await http::async_write(socket, response, use_awaitable);
                    } else { 
                        //NOT_MODIFIED
                        log_trace("Method: " << request.method() << " Url: " << request.target() << " NOT_MODIFIED");
                        http::response<http::string_body> response = http::response<http::string_body>{http::status::not_modified, request.version()};
                        response.version(request.version());
                        response.result(http::status::ok);
                        response.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                        response.set(http::field::content_type, "text/html");
                        response.keep_alive(request.keep_alive());
                        co_await http::async_write(socket, response, use_awaitable);
                    }
                } else {
                    //NOT_FOUND
                    log_trace("Method: " << request.method() << " Url: " << request.target() << " NOT_FOUND");
                    http::response<http::string_body> response = http::response<http::string_body>{http::status::not_found, request.version()};
                    response.version(request.version());
                    response.result(http::status::ok);
                    response.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                    response.set(http::field::content_type, "text/html");
                    response.keep_alive(request.keep_alive());
                    co_await http::async_write(socket, response, use_awaitable);
                }

                if (!keepAlive || !request.keep_alive()) {
                    break;
                }
            }
        }
    } catch (boost::system::system_error & ex1) {
        
    } catch (std::exception& ex0) {
        log_warn("" << ex0.what());
    }
    socket.shutdown(tcp::socket::shutdown_send);
    co_return;
}

int coroutine_http_server::start(short port,int hint) {
    
    std::condition_variable cv;
    std::mutex mtx;
    bool started = false;
    
    std::shared_ptr<boost::asio::io_context> ioc = std::make_shared<boost::asio::io_context>(hint);
    std::shared_ptr<std::thread> t = std::make_shared<std::thread>([&,port,ioc]() {
        
        co_spawn(*ioc,[&]() -> awaitable<void> {
            tcp::acceptor acceptor4(*ioc, tcp::endpoint(tcp::v4(), port));
            for (;;) {
                tcp::socket socket = co_await acceptor4.async_accept(use_awaitable);
                co_spawn(socket.get_executor(), this->session_handler(std::move(socket)), detached);
            }
        } , detached);
    
        // co_spawn(*ioc,[&]() -> awaitable<void> {
        // tcp::acceptor acceptor6(*ioc, tcp::endpoint(tcp::v6(), port));
        //     for (;;) {
        //         tcp::socket socket = co_await acceptor6.async_accept(use_awaitable);
        //         co_spawn(socket.get_executor(), this->session_handler(std::move(socket)), detached);
        //     }
        // } , detached);
        
        log_info("HTTP Server [" + std::to_string(port) + "] 已启动");
        started = true;
        cv.notify_one();
        ioc->run();
        log_info("HTTP Server [" + std::to_string(port) + "] 已停止");
    });

    std::unique_lock<std::mutex> lock(mtx);
    if (cv.wait_for(lock, std::chrono::seconds(3),[&]() { return started; })) {
        log_info("HTTP Server [" + std::to_string(port) + "] 启动成功");
        io_context = std::move(ioc);
        thread = std::move(t);
        return 0;
    } else {
        log_info("HTTP Server [" + std::to_string(port) + "] 启动超时");
        ioc->stop();
        t->join();
        return -1;
    }
}

void coroutine_http_server::stop() {
    log_info("正在关闭服务");
    if (io_context != nullptr && thread != nullptr && thread->joinable()) {
        io_context->stop();
        thread->join();
        log_info("已关闭服务");
    } else {
        log_error("服务未启动");
    }
}

void coroutine_http_server::add(http::verb method, std::string path, HttpFunction function) {
    auto methods = this->routes.find(path);
    if (methods == this->routes.end()) {
        this->routes[path][method] = function;
    } else {
        auto functions = methods->second.find(method);
        if (functions == methods->second.end()) {
            methods->second[method] = function;
        } else {
            log_warn(method << " " << path << " already exists");
            functions->second = function;
        }
    }
}\

coroutine_http_server coroutine_http_server::getTestInstance() {
    static coroutine_http_server httpd(100);
    httpd.add(http::verb::get, "/", 
            [](auto& socket,auto& request) -> awaitable<http::response<http::string_body>> {

            http::response<http::string_body> response = http::response<http::string_body>{ http::status::ok, request.version() };
            response.version(request.version());
            response.result(http::status::ok);
            response.body() = "Hello World";

            response.set(http::field::server, BOOST_BEAST_VERSION_STRING);
            response.set(http::field::content_type, "text/html");
            response.keep_alive(request.keep_alive());
            response.prepare_payload();
            co_return response;
        }
    );
    httpd.start(10086,100);
    sleep(1);
    return std::move(httpd);
}