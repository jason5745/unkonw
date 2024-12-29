//
// Created by 顾文超 on 2024/12/30.
//

#ifndef _REQUEST_H_
#define _REQUEST_H_

#include <string>
#include <unordered_map>
#include <google/protobuf/message.h>


class Request {

private:
    std::unordered_map<std::string_view ,std::string_view> params_;
    std::unordered_map<std::string_view ,std::string_view> headers_;
    google::protobuf::Message *proto_data_;
    std::string json_data_;
};

#endif //_REQUEST_H_
