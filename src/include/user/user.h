#include <vector>
#include "rocksdb/db.h"
#include "rocksdb/status.h"
#include <iostream>
#include "requestExceptions.h"
#include "UserToken.h"
#include "UserException.h"
#include "json/json.h"
#include <openssl/hmac.h>
#include <string.h>

#ifndef FDRIVE_BACKEND_USER_H
#define FDRIVE_BACKEND_USER_H

struct userFile {
    int id;
    std::string permits;
    std::string path;
};

// Class to represent a user.

class User {

    private:
        std::string email;
        std::string hashed_password;
        std::vector<UserToken*>* tokens;
        std::vector<struct userFile*>* files;
        bool checkIfExisting(rocksdb::DB* db, std::string* value);
        bool checkPassword(std::string password);

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

        // Methods to manage actions on files.
        void addFile(int id, std::string path);
        void addSharedFile(int id); // Los permisos son todo o nada.
        bool hasFile(int id);
        std::vector<struct userFile*> getFiles();
        bool isOwnerOfFile(int id);
        void eraseFile(int id);

        // Methods for loading and saving changes in the database.
        static User* load(rocksdb::DB* db, std::string email);
        bool save(rocksdb::DB* db);
};

#endif //FDRIVE_BACKEND_USER_H