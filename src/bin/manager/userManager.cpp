#include "userManager.h"
#include "file.h"
#include "fileManager.h"
#include "folder/folder.h"
#include <iostream>

int UserManager::counter = 0;

UserManager::UserManager() { }
UserManager::~UserManager() { }

std::string UserManager::createToken() {
    std::string token = "token" + std::to_string(counter);
    counter++;
    return token;
}

std::string UserManager::addUser(std::string email, std::string password) {
    User* user = new User();
    user->setEmail(email);
    user->setPassword(password);

    rocksdb::DB* db = NULL;

    try {
        db = this->openDatabase("En AddUser: ");
        ///std::cout << "Abrí la base de datos en AddUser." << std::endl;
        user->signup(db);
    } catch (std::exception& e) {
        delete user;
        if (db != NULL) {
            delete db;
            ///std::cout << "ERROR pero Cerré la base de datos en AddUser." << std::endl;
        }
        throw; // Needs to be this way. If you throw e, a new instance is created and the exception class is missed.
    }

    delete db;
    ///std::cout << "Cerré la base de datos en AddUser." << std::endl;
    delete user;

    return "{ \"result\" : true }";
}

std::string UserManager::loginUser(std::string email, std::string password) {
    rocksdb::DB* db = this->openDatabase("En LogIn: ");
    ///std::cout << "Abrí la base de datos en LoginUser." << std::endl;
    User* user = NULL;
    try {
        user = User::load(db, email);
        if (!user->login(password)) {
            delete user;
            delete db;
            ///std::cout << "ERROR pero Cerré la base de datos en LoginUser." << std::endl;
            throw WrongPasswordException();
        }
    } catch (std::exception& e) {
        if (user != NULL) delete user;
        delete db;
        ///std::cout << "ERROR pero Cerré la base de datos en LoginUser." << std::endl;
        throw; // Needs to be this way. If you throw e, a new instance is created and the exception class is missed.
    }

    std::string token;
    try {
        token = user->getNewToken(db);
    } catch (std::exception& e) {
        delete user;
        delete db;
        ///std::cout << "ERROR pero Cerré la base de datos en LoginUser." << std::endl;
        throw; // Needs to be this way. If you throw e, a new instance is created and the exception class is missed.
    }
    delete db;
    ///std::cout << "Cerré la base de datos en LoginUser." << std::endl;
    delete user;

    return "{ \"result\" : true , \"token\" : \"" + token + "\" }";
}

std::string UserManager::logoutUser(std::string email, std::string token) {
    rocksdb::DB* db = this->openDatabase("En LogOut: ");
    ///std::cout << "Abrí la base de datos en LogoutUser." << std::endl;
    User* user = NULL;
    try {
        user = User::load(db, email);
        if (! user->logout(db, token)) {
            delete user;
            delete db;
            ///std::cout << "ERROR pero Cerré la base de datos en LogoutUser." << std::endl;
            throw NotLoggedInException();
        }
    } catch (std::exception& e) {
        if (user != NULL) delete user;
        delete db;
        ///std::cout << "ERROR pero Cerré la base de datos en LogoutUser." << std::endl;
        throw; // Needs to be this way. If you throw e, a new instance is created and the exception class is missed.
    }
    delete db;
    ///std::cout << "Cerré la base de datos en LogoutUser." << std::endl;
    delete user;

    return "{ \"result\" : true }";
}

void UserManager::checkIfLoggedIn(std::string email, std::string token) {
    rocksdb::DB* db = openDatabase("En check if logged in");
    ///std::cout << "Abrí la base de datos en CheckIfLoggedIn." << std::endl;
    User* user= NULL;
    try {
        user = User::load(db, email);
        user->checkToken(token);
    } catch (std::exception& e) {
        if (user != NULL) delete user;
        delete db;
        ///std::cout << "ERROR pero Cerré la base de datos en CheckIfLoggedIn." << std::endl;
        throw; // Needs to be this way. If you throw e, a new instance is created and the exception class is missed.
    }
    delete user;
    delete db;
    ///std::cout << "Cerré la base de datos en CheckIfLoggedIn." << std::endl;
}

std::string UserManager::loadUserFiles(std::string email, std::string path) {
    rocksdb::DB* db = openDatabase("En load user files");
    ///std::cout << "Abrí la base de datos en LoadUserFiles." << std::endl;
    Folder* folder = NULL;
    try {
        folder = Folder::load(db, email, path);
    } catch (std::exception& e) {
        if (folder != NULL) delete folder;
        delete db;
        ///std::cout << "Cerré la base de datos en LoadUserFiles." << std::endl;
        throw; // Needs to be this way. If you throw e, a new instance is created and the exception class is missed.
    }

    std::string content = folder->getContent();
    delete folder;
    delete db;
    ///std::cout << "Cerré la base de datos en LoadUserFiles." << std::endl;
    return "{ \"result\" : true , \"content\" : " + content + " }";
}

std::string UserManager::getUsers(std::string email) {
    //TODO
    return "";
}