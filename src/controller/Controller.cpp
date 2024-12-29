//
// Created by qq574 on 2024/12/28.
//

#include <future>
#include "Logger.h"
#include "Controller.h"

void Controller::reflect(std::string_view name,
                         std::function<void(Controller *,google::protobuf::Message *,google::protobuf::Message *)> *invoke,
                         std::function<std::shared_ptr<google::protobuf::Message>()> *request,
                         std::function<std::shared_ptr<google::protobuf::Message>()> *response) {

    methods.insert({{name,Method([this,invoke](google::protobuf::Message *request,google::protobuf::Message *response) -> void {
        (*invoke)(this,request,response);
    },request,response)}});
}

Controller::Method *Controller::method(std::string_view name) {
    auto it = methods.find(name);
    if (it != methods.end()) {
        return &(it->second);
    } else {
        return nullptr;
    }
}