#ifndef __HttpRpcModule_H_
#define __HttpRpcModule_H_

#include <vector>
#include <memory>
#include <boost/asio.hpp>
#include <boost/thread/future.hpp>
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
	std::vector<std::unique_ptr<CircularQueue<ModuleRequest>>> i_ringbufs_;
	std::vector<std::unique_ptr<CircularQueue<ModuleRequest>>> o_ringbufs_;
public:
	HttpRpcModule();
	virtual ~HttpRpcModule();

	virtual void init(std::string_view configure) override;
	virtual void start() override;
	virtual void stop() override;
	virtual void exit() override;
	virtual std::shared_ptr<ModuleRequest> pop() override;
};

}

#endif