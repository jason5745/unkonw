//
// Created by qq574 on 2024/12/28.
//

#include "Logger.h"
#include "Controller.h"

void Controller::registerMethod(std::string_view name,uintptr_t function) {
    methods.insert({{name,function}});
}

void Controller::invokeMethod(std::string_view name,Request *request,Response *response) {
    auto it = methods.find(name);
    if (it != methods.end()) {
        (*(std::function<void(decltype(this),Request *,Response *)> *)(it->second))(this,request,response);
    } else {

    }
}