//
// Created by qq574 on 2024/12/28.
//

#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include <functional>
#include <functional>
#include <unordered_map>
#include "Singleton.h"
#include "Base.pb.h"

class Controller : public Singleton<Controller> {
private:
    std::unordered_map<std::string_view,uintptr_t> methods;
public:
    void registerMethod(std::string_view name,uintptr_t function);
    void invokeMethod(std::string_view name,Request *request,Response *response);
};


#endif //_CONTROLLER_H_
