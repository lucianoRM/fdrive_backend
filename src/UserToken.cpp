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

Json::Value UserToken::serialize() {
    Json::Value jsonToken;
    jsonToken["token"] = this->token;
    jsonToken["expiration"] = Json::Value::Int64((int64_t) this->expiration);
    return jsonToken;
}

static UserToken* deserialize(Json::Value jsonSerialization) {

    UserToken* userToken = new UserToken();
    userToken->expiration = jsonSerialization["expiration"].asInt64();
    userToken->token = jsonSerialization["token"].asString();
    return userToken;

}