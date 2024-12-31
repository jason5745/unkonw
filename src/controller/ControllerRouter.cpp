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

void ControllerRouter::reflect(std::string_view uri,
             std::function<void(std::shared_ptr<google::protobuf::Message>,std::shared_ptr<google::protobuf::Message>)> &invoke,
             std::function<std::shared_ptr<google::protobuf::Message>()> &req,
             std::function<std::shared_ptr<google::protobuf::Message>()> &res) {
    methods.insert({{uri,Method(invoke,req,res)}});
}

Method *ControllerRouter::find(std::string_view uri) {
    auto it = methods.find(uri);
    if (it != methods.end()) {
        return &(it->second);
    } else {
        return nullptr;
    }
}