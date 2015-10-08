//
// Created by agustin on 08/10/15.
//

#include "UserToken.h"

bool UserToken::hasExpired() {
    time_t currTime;
    time(&currTime);
    return difftime(currTime, this->expiration) < 0;
}