#include <vector>
#include "include/rocksdb/db.h"
#include "include/rocksdb/status.h"
#include <iostream>
#include "requestExceptions.h"
#include "UserToken.h"
#include "UserException.h"
#include "include/json/json.h"
#include <openssl/hmac.h>
#include <string.h>

#ifndef FDRIVE_BACKEND_USER_H
#define FDRIVE_BACKEND_USER_H

class User {

private:
    std::string email;
    std::string hashed_password;
    std::vector<UserToken*>* tokens;
    std::vector<int>* files;
    bool checkIfExisting(rocksdb::DB* db, std::string* value);
    bool checkPassword(std::string password);

public:

    User();
    ~User();

    std::string hashPassword(std::string password); // Left for testing purposes
    
    std::string getEmail();
    void setEmail (std::string email);

    void setPassword (std::string password);

    void signup(rocksdb::DB* db);
    bool signin(std::string password);

	bool addToken(rocksdb::DB* db, std::string token);
    void checkToken(rocksdb::DB* db,std::string token); //Checks if token is associated with user.

    void addFile(int id);   // != addSharedFile(permisos)
    std::vector<int> getFiles();
    
    static User* load(rocksdb::DB* db, std::string email);
    bool save(rocksdb::DB* db);
    
};

#endif //FDRIVE_BACKEND_USER_H
