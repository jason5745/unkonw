//
// Created by 顾文超 on 2024/12/30.
//

#ifndef _SYNCHYPERTEXTSERVICE_H_
#define _SYNCHYPERTEXTSERVICE_H_


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

class SyncHypertextServiceWorkHandler {
private:
    boost::asio::io_context ctx;
    std::thread th;
    std::size_t tid;
    std::size_t members = 0;

public:
    SyncHypertextServiceWorkHandler() = default;
    ~SyncHypertextServiceWorkHandler() = default;

    SyncHypertextServiceWorkHandler(const SyncHypertextServiceWorkHandler&) = delete;
    SyncHypertextServiceWorkHandler &operator=(const SyncHypertextServiceWorkHandler&) = delete;
    SyncHypertextServiceWorkHandler(SyncHypertextServiceWorkHandler &&) = delete;
    SyncHypertextServiceWorkHandler &operator=(SyncHypertextServiceWorkHandler &&) = delete;

    friend class SyncHypertextService;
};

class SyncHypertextService {
private:
    std::vector<SyncHypertextServiceWorkHandler> workers_;

public:
    SyncHypertextService(SyncHypertextService &&other) noexcept {
        workers_ = std::move(other.workers_);
    }

    SyncHypertextService &operator=(SyncHypertextService &&other) noexcept {
        if (this != &other) {
            workers_ = std::move(other.workers_);
        }
        return *this;
    }
    SyncHypertextService(size_t threads);
    ~SyncHypertextService();
    int start(uint16_t port);
    void stop();
    awaitable<void> session(SyncHypertextServiceWorkHandler *worker, tcp::socket socket);
};

#endif //SYNCHYPERTEXTSERVICE_H_
