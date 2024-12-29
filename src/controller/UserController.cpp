//
// Created by qq574 on 2024/12/28.
//
#include <google/protobuf/util/json_util.h>
#include "UserController.h"
#include "router/Reflect.h"

REGISTER_CLASS("user", UserController);



REGISTER_CLASS_METHOD(UserController, login);
void UserController::login(Request &request,Response &response) {
    std::string tmp;
    auto ret = google::protobuf::util::MessageToJsonString(request,&tmp);

    std::cout << "login" << std::endl;
}


REGISTER_CLASS_METHOD(UserController, logout);
void UserController::logout(Request &request,Response &response) {
    std::cout << "logout" << std::endl;
}


REGISTER_CLASS_METHOD(UserController, keepalive);
void UserController::keepalive(Request &request,Response &response) {

}