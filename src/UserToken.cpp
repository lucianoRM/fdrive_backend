//
// Created by agustin on 08/10/15.
//

#include <iostream>
#include "UserToken.h"

UserToken::UserToken() { }

UserToken::UserToken(std::string token) {
    this->token = token;
    time_t currTime;
    time(&currTime);
    this->expiration = currTime;
    this->expiration += TIME;
}


bool UserToken::hasExpired(time_t* currTime) {
    if (currTime == NULL) {
        time_t _currTime;
        currTime = &_currTime;
        time(&_currTime);
    }
    return difftime(this->expiration, *currTime) < 0;
}

Json::Value UserToken::serialize() {
    Json::Value jsonToken;
    jsonToken["token"] = this->token;
    jsonToken["expiration"] = Json::Value::Int64((int64_t) this->expiration);
    return jsonToken;
}

UserToken* UserToken::deserialize(Json::Value jsonSerialization) {

    UserToken* userToken = new UserToken();
    userToken->expiration = jsonSerialization["expiration"].asInt64();
    userToken->token = jsonSerialization["token"].asString();
    return userToken;

}

UserToken* UserToken::getRandomToken() {
    static const char alphanum[] =
            "0123456789"
                    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                    "abcdefghijklmnopqrstuvwxyz";
    int len = 20;
    std::string token = "12345678901234567890";
    for (int i = 0; i < len; ++i) {
        token[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    return new UserToken(token);
}