//
// Created by qq574 on 2024/12/28.
//

#ifndef _REFLECT_H_
#define _REFLECT_H_

#include "ControllerRouter.h"
#include "controller/Controller.h"

#define REGISTER_CLASS(url,clazz)                                                                                                       \
    static ControllerReflect reflect##clazz(url, clazz::getInstance())

#define REGISTER_CLASS_METHOD(clazz, method, typeq, typer)                                                                              \
    static std::function<void(Controller *,google::protobuf::Message *,google::protobuf::Message *)> _reflectInvoke##clazz##method =    \
    [](Controller *controller,google::protobuf::Message *request,google::protobuf::Message *response) -> void {                         \
        static_cast<clazz *>(controller)->method(dynamic_cast<typeq *>(request),dynamic_cast<typer *>(response));                       \
    };                                                                                                                                  \
    static std::function<std::shared_ptr<google::protobuf::Message>()> _reflectRequest##clazz##method = []() ->                                        \
        std::shared_ptr<google::protobuf::Message> {return std::make_shared<typeq>();};                                                 \
    static std::function<std::shared_ptr<google::protobuf::Message>()> _reflectResponse##clazz##method = []() ->                                       \
        std::shared_ptr<google::protobuf::Message> {return std::make_shared<typer>();};                                                 \
    static ControllerReflect reflect##clazz##method(clazz::getInstance(), #method,                                                      \
                                                    &_reflectInvoke##clazz##method,                                                     \
                                                    &_reflectRequest##clazz##method,                                                    \
                                                    &_reflectResponse##clazz##method)


class ControllerReflect {
public:
    ControllerReflect(std::string_view name, Controller *controller) {
        ControllerRouter::getInstance()->registerController(name, controller);
    }
    ControllerReflect(Controller *controller,
                      std::string_view name,
                      std::function<void(Controller *,google::protobuf::Message *,google::protobuf::Message *)> *invoke,
                      std::function<std::shared_ptr<google::protobuf::Message>()> *request,
                      std::function<std::shared_ptr<google::protobuf::Message>()> *response) {
        controller->reflect(name,invoke,request,response);
    }
};






#endif //_REFLECT_H_
