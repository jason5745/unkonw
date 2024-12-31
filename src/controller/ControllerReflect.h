//
// Created by qq574 on 2024/12/28.
//

#ifndef _REFLECT_H_
#define _REFLECT_H_

#include "ControllerRouter.h"
#include "controller/Controller.h"

#define REGISTER_CLASS_METHOD(uri, clazz, method, typeReq, typeRes)                                                                     \
    static ControllerReflect reflect##clazz##method(uri,                                                                                \
        [](std::shared_ptr<google::protobuf::Message> request, std::shared_ptr<google::protobuf::Message> response) -> void {           \
            UserController::login(dynamic_cast<typeReq *>(request.get()),dynamic_cast<typeRes *>(response.get())); },                   \
        []() -> std::shared_ptr<google::protobuf::Message> { return std::make_shared<typeReq>(); },                                     \
        []() -> std::shared_ptr<google::protobuf::Message> { return std::make_shared<typeRes>(); })                                     \


class ControllerReflect {
public:
    ControllerReflect(std::string_view uri,
                      std::function<void(std::shared_ptr<google::protobuf::Message>,std::shared_ptr<google::protobuf::Message>)> invoke,
                      std::function<std::shared_ptr<google::protobuf::Message>()> req,
                      std::function<std::shared_ptr<google::protobuf::Message>()> res) {
        ControllerRouter::getInstance()->reflect(uri,invoke,req,res);
    }
};






#endif //_REFLECT_H_
