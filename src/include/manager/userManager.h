#include "mongoose.h"
#include "user.h"
#include <iostream>


#ifndef TALLER_USERMANAGER_H
#define TALLER_USERMANAGER_H

#include "Manager.h"


// Class to manage the requests related to users.

class UserManager:public Manager {

    private:
        static int counter;
        std::string createToken();

    public:
        UserManager();
        ~UserManager();

        bool checkExistentUser(std::string email); // Devuelve true si existe.
        std::string addUser(std::string email, std::string password);
        void checkIfLoggedIn(std::string email, std::string token);
        std::string loginUser(std::string email, std::string password);
        std::string logoutUser(std::string email, std::string token);
        std::string loadUserFiles(std::string email, std::string path);
        void checkIfUserHasFilePermits(std::string email, int id);
        void checkFileAddition(std::string email, int size);
        void addFile(std::string email, int size);
        void checkFileSizeChange(std::string email, int oldSize, int newSize);
        void changeFileSize(std::string email, int oldSize, int newSize);
        // Returns all user's metadata except the one it receives.
        std::string getUsers(std::string email);
        std::string saveUserData(std::string email, std::string name, std::string lastLocation);
};

#endif //TALLER_USERMANAGER_H
