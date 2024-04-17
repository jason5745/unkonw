
#include "coroutine_tcp_server.hpp"
#include "coroutine_http_server.hpp"
#include "log.hpp"

static std::string ByteToHexString(const char* data, size_t length) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (size_t i = 0; i < length; ++i) {
        ss << std::setw(2) << static_cast<int>(static_cast<unsigned char>(data[i]));
    }
    return ss.str();
}

int main(int argc, char **argv) {
    logger::configure(boost::log::trivial::trace, 1);
    coroutine_tcp_server server(
        [] (tcp::socket &socket) -> awaitable<void> {
            log_info(socket.remote_endpoint().address().to_string() 
                << ":" 
                << socket.remote_endpoint().port() 
                << " 已连接");
            co_return;
        },
        [](tcp::socket &socket,const char * data,size_t size) -> awaitable<void> {
            log_info(socket.remote_endpoint().address().to_string()
                << ":"
                << socket.remote_endpoint().port() 
                << " 收到数据: " << ByteToHexString(data,size));

            co_await socket.async_write_some(boost::asio::buffer(data, size),use_awaitable);
            log_info("数据已发送");
            co_return;
        },
        [](tcp::socket &socket) -> awaitable<void> {
            log_info(socket.remote_endpoint().address().to_string() 
                << ":" 
                << socket.remote_endpoint().port() 
                << " 已断开");
            co_return;
        }
    );

    coroutine_http_server httpd(100);
    httpd.add(http::verb::get, "/", 
            [](auto& socket,auto& request) -> awaitable<http::response<http::string_body>> {
            http::response<http::string_body> response = http::response<http::string_body>{ http::status::ok, request.version() };
            response.version(request.version());
            response.result(http::status::ok);
            response.set(http::field::server, BOOST_BEAST_VERSION_STRING);
            response.set(http::field::content_type, "text/html");
            response.keep_alive(request.keep_alive());
            
            co_return response;
        }
    );
    httpd.start(10085);
    server.start(10086,1);

    sleep(60 * 60);

    server.stop();
    httpd.stop();

    return 0;
}
