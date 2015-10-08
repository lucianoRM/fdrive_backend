//
// Created by agustin on 08/10/15.
//

#ifndef FDRIVE_BACKEND_USERTOKEN_H
#define FDRIVE_BACKEND_USERTOKEN_H

#include <string>

class UserToken {
public:
    std::string token;
    time_t expiration;
    bool hasExpired();
};

#endif //FDRIVE_BACKEND_USERTOKEN_H
