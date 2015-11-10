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
        int totalQuota;  // In MB.
        int freeQuota;
        std::vector<UserToken*>* tokens;

        bool checkIfExisting(rocksdb::DB* db, std::string* value);
        bool checkPassword(std::string password);
        bool setFileStructure(rocksdb::DB* db);
        std::string getJsonFileStructure();
        bool saveInUsers(rocksdb::DB* db);
        bool deleteFromUsers(rocksdb::DB* db);


    public:
        User();
        ~User();

        std::string hashPassword(std::string password); // Left for testing purposes

        std::string getEmail();

        // SETTERS.
        void setEmail (std::string email);
        void setPassword (std::string password);
        void setName (std::string name);
        void setLocation (std::string location);
        void setProfilePicturePath(std::string path);

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

        // Method for obtaining Json information of the user
        std::string getJson();
        Json::Value getJsonValue();
        
        //Method for quotas
        bool addFileOfSize(int size);	// Size must be in MB.
        bool changeSizeOfFile(int oldSize, int newSize);
};

#endif //FDRIVE_BACKEND_USER_H
