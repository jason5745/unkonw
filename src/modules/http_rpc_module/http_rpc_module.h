#ifndef __HttpRpcModule_H_
#define __HttpRpcModule_H_

#include <vector>
#include <boost/asio.hpp>
#include "boost_coroutine_http_server.h"
#include "logger.h"
#include "module.h"

namespace module {
using namespace boost::beast::http;
using ::boost::asio::awaitable;
using ::boost::asio::co_spawn;
using ::boost::asio::detached;
using ::boost::asio::use_awaitable;

class HttpRpcModule : public Module {
private:
    int threads_;
    int pop_balance_;
    std::unique_ptr<utils::boost::coroutine::http::Server> service_;
    std::vector<std::unique_ptr<RingBuf<std::unique_ptr<ModuleRequest>>>> i_ringbufs_;
    std::vector<std::unique_ptr<RingBuf<std::unique_ptr<ModuleRequest>>>> o_ringbufs_;
public:
    HttpRpcModule();
    ~HttpRpcModule();
    awaitable<response<string_body>> handle(int thread_num, auto &socket, request<string_body> &request);
    virtual void init(std::string_view configure) override;
    virtual void start() override;
    virtual void stop() override;
    virtual void exit() override;
    virtual bool pop(std::unique_ptr<ModuleRequest> &request) override;
};

}

#endif