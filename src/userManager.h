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
        int counter = 0;
        std::string createToken();

    public:
        UserManager();
        ~UserManager();

        std::string addUser(std::string email, std::string password);
        void checkIfLoggedIn(std::string email, std::string token);
        std::string loginUser(std::string email, std::string password);
};


#endif //TALLER_USERMANAGER_H
