//
// Created by agustin on 10/09/15.
//

#include <string>
#include <vector>
#include "rocksdb/db.h"

#ifndef FDRIVE_BACKEND_USER_H
#define FDRIVE_BACKEND_USER_H

class User {

private:
    std::string email;
    std::string hashed_password;
    //std::vector<> tokens;
    bool checkIfExisting(rocksdb::DB* db, std::string* value);

public:

    User(std::string email, std::string password);
    void setPassword(std::string password);
    
    std::string getEmail();
    
    std::string hashPassword(std::string password);
    bool signup(rocksdb::DB* db);
    bool checkPassword(std::string password);
    bool save(rocksdb::DB* db);
	bool addToken(rocksdb::DB* db, std::string token);
    
    bool load(rocksdb::DB* db);
    
};

#endif //FDRIVE_BACKEND_USER_H
