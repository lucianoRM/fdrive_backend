//
// Created by agustin on 10/09/15.
//

#include <string>
#include "rocksdb/db.h"

#ifndef FDRIVE_BACKEND_USER_H
#define FDRIVE_BACKEND_USER_H

class User {

private:
    std::string email;
    std::string hashed_password;

public:
    bool signup(rocksdb::DB* db);
    void setEmail(std::string email);
    std::string getEmail();
    void setPassword(std::string password);
    bool checkPassword(std::string password);

    bool save();
    User* get(std::string email);
};

#endif //FDRIVE_BACKEND_USER_H
