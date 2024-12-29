//
// Created by qq574 on 2024/12/28.
//

#include "ControllerRouter.h"

static std::vector<std::string_view> split(std::string_view &s, char delimiter) {
    std::vector<std::string_view> tokens;
    size_t start = 0;
    size_t end = s.find(delimiter);

    while (end != std::string::npos) {
        tokens.push_back(s.substr(start, end - start));
        start = end + 1;
        end = s.find(delimiter, start);
    }
    tokens.push_back(s.substr(start)); // 添加最后一个片段
    return tokens;
}

void ControllerRouter::registerController(std::string_view name, Controller *controller) {
    controllers.insert({{name,controller}});
}

Controller *ControllerRouter::getController(std::string_view name) {
    auto it = controllers.find(name);
    if (it != controllers.end()) {
        return it->second;
    } else {
        return nullptr;
    }
}
void ControllerRouter::invokeMethod(std::string_view &url, BasicRequest *request, BasicResponse *response) {
    auto fragments = split(url,'/');

}