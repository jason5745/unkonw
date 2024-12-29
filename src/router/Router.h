//
// Created by qq574 on 2024/12/28.
//

#ifndef _ROUTER_H_
#define _ROUTER_H_

#include <unordered_map>
#include "Singleton.h"
#include "controller/Controller.h"

class Router : public Singleton<Router> {
private:
    std::unordered_map<std::string_view ,Controller *> controllers;
public:
    void registerController(std::string_view name,Controller *controller);
    Controller *getController(std::string_view name);
};

#endif //_ROUTER_H_
