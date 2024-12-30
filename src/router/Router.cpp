//
// Created by qq574 on 2024/12/28.
//

#include "Router.h"

void Router::registerController(std::string_view name,Controller *controller) {
    controllers.insert({{name,controller}});
}

Controller *Router::getController(std::string_view name) {
    auto it = controllers.find(name);
    if (it != controllers.end()) {
        return it->second;
    } else {
        return nullptr;
    }
}