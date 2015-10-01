//
// Created by luciano on 29/09/15.
//

#include "mongoose.h"
#include "user.h"
#include <iostream>


#ifndef TALLER_USERMANAGER_H
#define TALLER_USERMANAGER_H


class UserManager {

    private:
        int counter = 0;

    private:
        std::string createToken();

    public:
        UserManager();
        ~UserManager();




        ///All public methods should have the same signature.
        int addUser(struct mg_connection *conn);

        int userLogin(struct mg_connection *conn);
};


#endif //TALLER_USERMANAGER_H
