//
// Created by 顾文超 on 2024/12/30.
//

#ifndef _AwaitHYPERTEXTSERVICE_H_
#define _AwaitHYPERTEXTSERVICE_H_


#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <unordered_map>

using boost::asio::ip::tcp;
using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;
namespace http = boost::beast::http;

class AwaitHypertextServiceWorkHandler {
private:
    boost::asio::io_context ctx;
    std::thread th;
    std::size_t tid;
    std::size_t members = 0;

public:
    AwaitHypertextServiceWorkHandler() = default;
    ~AwaitHypertextServiceWorkHandler() = default;

    AwaitHypertextServiceWorkHandler(const AwaitHypertextServiceWorkHandler&) = delete;
    AwaitHypertextServiceWorkHandler &operator=(const AwaitHypertextServiceWorkHandler&) = delete;
    AwaitHypertextServiceWorkHandler(AwaitHypertextServiceWorkHandler &&) = delete;
    AwaitHypertextServiceWorkHandler &operator=(AwaitHypertextServiceWorkHandler &&) = delete;

    friend class AwaitHypertextService;
};

class AwaitHypertextService {
private:
    std::vector<AwaitHypertextServiceWorkHandler> workers;

public:
    AwaitHypertextService(AwaitHypertextService &&other) noexcept {
        workers = std::move(other.workers);
    }

    AwaitHypertextService &operator=(AwaitHypertextService &&other) noexcept {
        if (this != &other) {
            workers = std::move(other.workers);
        }
        return *this;
    }
    AwaitHypertextService(size_t threads);
    ~AwaitHypertextService();
    int start(uint16_t port);
    void stop();
    awaitable<void> session(AwaitHypertextServiceWorkHandler *worker, tcp::socket socket);
};

#endif //AwaitHYPERTEXTSERVICE_H_
