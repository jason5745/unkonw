//
// Created by qq574 on 2024/12/28.
//

#ifndef _ROUTER_H_
#define _ROUTER_H_

#include <unordered_map>
#include "Singleton.h"
#include "controller/Controller.h"

class Method {
public:
    std::function<void(std::shared_ptr<google::protobuf::Message>,std::shared_ptr<google::protobuf::Message>)> invoke;
    std::function<std::shared_ptr<google::protobuf::Message>()> req;
    std::function<std::shared_ptr<google::protobuf::Message>()> res;

    Method(std::function<void(std::shared_ptr<google::protobuf::Message>,std::shared_ptr<google::protobuf::Message>)> &_invoke,
           std::function<std::shared_ptr<google::protobuf::Message>()> &_req,
           std::function<std::shared_ptr<google::protobuf::Message>()> &_res) : invoke(std::move(_invoke)),req(std::move(_req)),res(std::move(_res)) {};
};

class ControllerRouter : public Singleton<ControllerRouter> {
public:
    void reflect(std::string_view uri,
                 std::function<void(std::shared_ptr<google::protobuf::Message>,std::shared_ptr<google::protobuf::Message>)> &invoke,
                 std::function<std::shared_ptr<google::protobuf::Message>()> &req,
                 std::function<std::shared_ptr<google::protobuf::Message>()> &res);

    Method *find(std::string_view uri);
private:
    std::unordered_map<std::string_view,Method> methods;

};

#endif //_ROUTER_H_
