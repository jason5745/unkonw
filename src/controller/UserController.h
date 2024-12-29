//
// Created by qq574 on 2024/12/28.
//

#ifndef _USERCONTROLLER_H_
#define _USERCONTROLLER_H_

#include "Controller.h"

class UserController : public Controller {

public:
    void login(Request &request,Response &response);
    void logout(Request &request,Response &response);
    void keepalive(Request &request,Response &response);

};


#endif //_USERCONTROLLER_H_
