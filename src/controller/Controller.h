//
// Created by qq574 on 2024/12/28.
//

#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include <functional>
#include <functional>
#include <unordered_map>
#include <utility>
#include <google/protobuf/util/json_util.h>
#include "Singleton.h"


class Controller : public Singleton<Controller> {
    class Method {
    public:
        std::function<void(google::protobuf::Message *,google::protobuf::Message *)> invoke;
            std::function<std::shared_ptr<google::protobuf::Message>()> request;
            std::function<std::shared_ptr<google::protobuf::Message>()> response;

        Method(std::function<void(google::protobuf::Message *,google::protobuf::Message *)> _invoke,
               std::function<std::shared_ptr<google::protobuf::Message>()> *_request,
               std::function<std::shared_ptr<google::protobuf::Message>()> *_response): invoke(std::move(_invoke)),request(*_request),response(*_response) {};
};

private:
    std::unordered_map<std::string_view,Method> methods;
public:
    void reflect(std::string_view name,
                 std::function<void(Controller *,google::protobuf::Message *,google::protobuf::Message *)> *invoke,
                 std::function<std::shared_ptr<google::protobuf::Message>()> *request,
                 std::function<std::shared_ptr<google::protobuf::Message>()> *response);
    Controller::Method *method(std::string_view name);
};


#endif //_CONTROLLER_H_
