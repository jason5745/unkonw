//
// Created by qq574 on 2024/12/28.
//

#ifndef _ROUTER_H_
#define _ROUTER_H_

#include <unordered_map>
#include "Singleton.h"
#include "Basic.pb.h"
#include "controller/Controller.h"

class ControllerRouter : public Singleton<ControllerRouter> {
private:
    std::unordered_map<std::string_view ,Controller *> controllers;
public:
    void registerController(std::string_view name,Controller *controller);
    Controller *getController(std::string_view name);
    void invokeMethod(std::string_view &url,BasicRequest *request,BasicResponse *response);
};

#endif //_ROUTER_H_
