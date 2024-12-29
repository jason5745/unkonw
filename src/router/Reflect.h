//
// Created by qq574 on 2024/12/28.
//

#ifndef _REFLECT_H_
#define _REFLECT_H_

#include "Router.h"
#include "controller/Controller.h"

#define REGISTER_CLASS(url,clazz)                                                                                   \
    static Reflect reflect##clazz(url, clazz::getInstance())

#define REGISTER_CLASS_METHOD(clazz, method)                                                                        \
    static std::function<void(clazz *,Request &,Response &)> reflectMethod##clazz##method = &clazz::method;                \
    static Reflect reflect##clazz##method(clazz::getInstance(), #method, (uintptr_t)&reflectMethod##clazz##method)

class Reflect {
public:
    Reflect(std::string_view name,Controller *controller) {
        Router::getInstance()->registerController(name,controller);
    }
    Reflect(Controller *controller,std::string_view name,uintptr_t function) {
        controller->registerMethod(name,function);
    }
};






#endif //_REFLECT_H_
