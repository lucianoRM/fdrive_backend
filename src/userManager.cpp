#include "userManager.h"

UserManager::UserManager(){}
UserManager::~UserManager(){}

std::string UserManager::createToken(){

    std::string token = "token" + std::to_string(counter);
    counter++;
    return token;

}

std::string UserManager::addUser(std::string email, std::string password){

    User* user = new User();
    user->setEmail(email);
    user->setPassword(password);

    rocksdb::DB* db = this->openDatabase("En AddUser: ");

    try {
        user->signup(db);
    }catch(std::exception& e){
        delete db;
        throw; //Needs to be this way. If you throw e, a new instance is created and the exception class is missed
    }

    delete db;
    delete user;

    return "{ \"result\" : true }";
}



std::string UserManager::loginUser(std::string email, std::string password){

    rocksdb::DB* db = this->openDatabase("En LogIn: ");

    User* user;
    try{
        user = User::load(db, email);
        if (!user->signin(password)) throw;
    }catch(std::exception& e){
        delete db;
        throw; //Needs to be this way. If you throw e, a new instance is created and the exception class is missed
    }
    delete db;

    std::string token = createToken();

    db = this->openDatabase("En LogIn: ");

    try{
        user->addToken(db, token);
    }catch(std::exception& e){
        delete db;
        throw; //Needs to be this way. If you throw e, a new instance is created and the exception class is missed
    }
    delete db;
    delete user;

    return "{ \"result\" : \"true\" , \"token\" : " + token + " }";
}


void UserManager::checkIfLoggedIn(std::string email, std::string token){
    rocksdb::DB* db = openDatabase("En check if logged in");

    try {
        User* user = User::load(db, email);
        user->checkToken(db, token);
    }catch(std::exception& e){
        delete db;
        throw; //Needs to be this way. If you throw e, a new instance is created and the exception class is missed
    }

    delete db;
}
