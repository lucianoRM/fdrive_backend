//
// Created by agustin on 08/10/15.
//

#ifndef FDRIVE_BACKEND_USERTOKEN_H
#define FDRIVE_BACKEND_USERTOKEN_H

#include <string>
#include "include/json/json.h"

class UserToken {

private:
    time_t TIME = 180000;

public:
    UserToken();
    UserToken(std::string token);
    std::string token;
    time_t expiration;
    bool hasExpired();
    Json::Value serialize();
};

#endif //FDRIVE_BACKEND_USERTOKEN_H
