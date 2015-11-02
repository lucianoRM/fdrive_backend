#include <vector>
#include "rocksdb/db.h"
#include "rocksdb/status.h"
#include <iostream>
#include "exceptions/requestExceptions.h"
#include "exceptions/dbExceptions.h"
#include "UserToken.h"
#include "exceptions/userExceptions.h"
#include "json/json.h"
#include <openssl/hmac.h>
#include <string.h>

#ifndef FDRIVE_BACKEND_USER_H
#define FDRIVE_BACKEND_USER_H

// Class to represent a user.

class User {

    private:
        std::string email;
        std::string hashed_password;
        std::string name;
        std::string lastLocation;
        std::string picture;    // Path to the profile picture.
        int quota;  // In MB.
        std::vector<UserToken*>* tokens;
        bool checkIfExisting(rocksdb::DB* db, std::string* value);
        bool checkPassword(std::string password);
        bool setFileStructure(rocksdb::DB* db);
        // If val == 1 is the structure for root folder.
        // If not. Is the structure for share and trash folder.
        std::string getJsonFileStructure(int val);

    public:
        User();
        ~User();

        std::string hashPassword(std::string password); // Left for testing purposes

        std::string getEmail();

        // SETTERS.
        void setEmail (std::string email);
        void setPassword (std::string password);

        // Methods related to logging.
        void signup(rocksdb::DB* db);
        bool login(std::string password);
        bool logout(rocksdb::DB* db, std::string token);

        // Methods related to the tokens received.
        void checkToken(std::string token); //Checks if token is associated with user.
        void deleteExpiredTokens(time_t* currTime = NULL);
        std::string getNewToken(rocksdb::DB* db);

        // Methods for loading and saving changes in the database.
        static User* load(rocksdb::DB* db, std::string email);
        bool save(rocksdb::DB* db);
};

#endif //FDRIVE_BACKEND_USER_H