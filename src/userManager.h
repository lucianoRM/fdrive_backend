//
// Created by luciano on 29/09/15.
//

#include "mongoose.h"
#include "user.h"
#include <iostream>


#ifndef TALLER_USERMANAGER_H
#define TALLER_USERMANAGER_H

#include "Manager.h"
#include "include/rocksdb/db.h"

class UserManager:public Manager {

    private:
        static int counter;
        std::string createToken();

    public:
        UserManager();
        ~UserManager();

        std::string addUser(std::string email, std::string password);
        void checkIfLoggedIn(std::string email, std::string token);
        std::string loginUser(std::string email, std::string password);
        std::string logoutUser(std::string email, std::string token);
        void addFileToUser(std::string email, int id);
        void addFileToUserAsOwner(std::string email, int id);
        std::string loadUserFiles(std::string email);
};


#endif //TALLER_USERMANAGER_H
