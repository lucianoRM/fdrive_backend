//
// Created by agustin on 10/09/15.
//

#include <string>
#include <vector>
#include "rocksdb/db.h"
#include <iostream>
#include "json/json.h"
#include "json/json-forwards.h"
#include <iostream>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/engine.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include "requestExceptions.h"

#ifndef FDRIVE_BACKEND_USER_H
#define FDRIVE_BACKEND_USER_H

class User {

private:
    std::string email;
    std::string hashed_password;
    //std::vector<> tokens;
    bool checkIfExisting(rocksdb::DB* db, std::string* value);

public:

    User();
    
    std::string getEmail();
    
    std::string hashPassword(std::string password);
    void signup(rocksdb::DB* db);
    void setEmail (std::string email);
    void setPassword (std::string password);
    bool signin(std::string password);
    bool checkPassword(std::string password);
    bool save(rocksdb::DB* db);
	bool addToken(rocksdb::DB* db, std::string token);
    void checkToken(rocksdb::DB* db,std::string token); //Checks if token is associated with user.
    
    static User* load(rocksdb::DB* db, std::string email);
    
};

#endif //FDRIVE_BACKEND_USER_H
