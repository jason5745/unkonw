//
// Created by qq574 on 2024/12/28.
//

#include <google/protobuf/util/json_util.h>
#include "UserController.h"
#include "ControllerReflect.h"


REGISTER_CLASS("user", UserController);
REGISTER_CLASS_METHOD(UserController, login, LoginRequest, LoginResponse);
void UserController::login(LoginRequest *request,LoginResponse *response) {
    response->set_code(0);
    response->set_desc("1234");
    response->set_allocated_data(nullptr);
}

//REGISTER_CLASS_METHOD(UserController, logout);
//void UserController::logout(google::protobuf::Message *request,google::protobuf::Message *response) {
//    std::cout << "logout" << std::endl;
//}
//
//
//REGISTER_CLASS_METHOD(UserController, keepalive);
//void UserController::keepalive(google::protobuf::Message *request,google::protobuf::Message *response) {
//
//}