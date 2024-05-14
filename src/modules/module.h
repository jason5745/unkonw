#ifndef __MODULE_H_
#define __MODULE_H_

#include <future>
#include <string>
#include <memory>
#include <ringbuf.h>
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

};

class Module {
private:
public:
    Module() {};
    ~Module() {};

    virtual void init(std::string_view configure) {};
    virtual void start() {};
    virtual void stop() {};
    virtual void exit() {};
    virtual bool pop(std::unique_ptr<ModuleRequest> &request) { return false; };
    virtual bool push(std::unique_ptr<ModuleRequest> &request) { return false; };
};

}

#endif