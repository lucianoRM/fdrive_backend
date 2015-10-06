//
// Created by luciano on 29/09/15.
//

#include "mongoose.h"
#include "user.h"
#include <iostream>


#ifndef TALLER_USERMANAGER_H
#define TALLER_USERMANAGER_H

#include "Manager.h"

class UserManager:public Manager {

    private:
        int counter = 0;
        std::string createToken();

    public:
        UserManager();
        ~UserManager();

        ///All public methods should have the same signature.
        int addUser(struct mg_connection *conn);

        int userLogin(struct mg_connection *conn);
};


#endif //TALLER_USERMANAGER_H
