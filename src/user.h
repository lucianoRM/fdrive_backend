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
#include "errorManager.h"

#ifndef FDRIVE_BACKEND_USER_H
#define FDRIVE_BACKEND_USER_H

class User {

private:
    std::string email;
    std::string hashed_password;
    //std::vector<> tokens;
    bool checkIfExisting(rocksdb::DB* db, std::string* value);

public:

    User(std::string email);
    
    std::string getEmail();
    
    std::string hashPassword(std::string password);
    bool signup(rocksdb::DB* db, std::string password);
    bool checkPassword(rocksdb::DB* db, std::string password);
    bool save(rocksdb::DB* db);
	bool addToken(rocksdb::DB* db, std::string token);
    void checkToken(rocksdb::DB* db,std::string token); //Checks if token is associated with user.
    
    bool load(rocksdb::DB* db, std::string password);
    
};

#endif //FDRIVE_BACKEND_USER_H
