#ifndef __MODULE_H_
#define __MODULE_H_

#include <future>
#include <string>
#include <memory>
#include "circular_queue.h"
#include "general_service.pb.h"

namespace module {
class ModuleRequest {
private:
	GeneralService::Request request_;
public:
	std::function<void(GeneralService::Response &&)> future_;
	ModuleRequest() = default;
	ModuleRequest(GeneralService::Request &&request, std::function<void(GeneralService::Response &&)> &&future) {
		request_ = std::move(request);
		future_ = std::move(future);
	};
	~ModuleRequest() = default;
	//禁止拷贝构造
	ModuleRequest(const ModuleRequest &) = delete;
	ModuleRequest &operator=(const ModuleRequest &) = delete;

	ModuleRequest(ModuleRequest &&other) noexcept {
		if (this != &other) {
			request_ = std::move(other.request_);
			future_ = std::move(other.future_);
		}
	};
	ModuleRequest &operator=(const ModuleRequest &&other) noexcept {
		if (this != &other) {
			request_ = std::move(other.request_);
			future_ = std::move(other.future_);
		}
		return *this;
	};

};

class Module {
private:
public:
	Module() {};
	virtual ~Module() {};
	virtual void init(std::string_view configure) {};
	virtual void start() {};
	virtual void stop() {};
	virtual void exit() {};
	virtual bool push(std::shared_ptr<ModuleRequest> &request) { return false; };
	virtual std::shared_ptr<ModuleRequest> pop() {
		return nullptr;
	};
};
}

#endif