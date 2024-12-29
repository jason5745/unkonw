//
// Created by qq574 on 2024/12/28.
//

#ifndef _USERCONTROLLER_H_
#define _USERCONTROLLER_H_

#include "Controller.h"
#include "User.pb.h"

class UserController : public Controller {

public:
    void login(LoginRequest *request,LoginResponse *response);
    void logout(google::protobuf::Message *request,google::protobuf::Message *response);
    void keepalive(google::protobuf::Message *request,google::protobuf::Message *response);
};


#endif //_USERCONTROLLER_H_
