//
// Created by agustin on 08/10/15.
//

#include "UserToken.h"

UserToken::UserToken() { }

UserToken::UserToken(std::string token) {
    this->token = token;
    time(&this->expiration);
    this->expiration += TIME;
}


bool UserToken::hasExpired() {
    //time_t currTime;
    //time(&currTime);
    //return difftime(currTime, this->expiration) < 0;
    return false;
}