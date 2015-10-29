#include "userManager.h"
#include "file.h"
#include "fileManager.h"

int UserManager::counter = 0;

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
        if (!user->login(password)) throw WrongPasswordException();
    }catch(std::exception& e){
        delete db;
        throw; //Needs to be this way. If you throw e, a new instance is created and the exception class is missed
    }
    delete db;

    db = this->openDatabase("En LogIn: ");

    std::string token;

    try{
        token = user->getNewToken(db);
    }catch(std::exception& e){
        delete db;
        throw; //Needs to be this way. If you throw e, a new instance is created and the exception class is missed
    }
    delete db;
    delete user;

    return "{ \"result\" : true , \"token\" : \"" + token + "\" }";
}


std::string UserManager::logoutUser(std::string email, std::string token) {
    rocksdb::DB* db = this->openDatabase("En LogOut: ");

    User* user;
    try{
        user = User::load(db, email);
        if (!user->logout(db, token)) throw NotLoggedInException();
    }catch(std::exception& e){
        delete db;
        throw; //Needs to be this way. If you throw e, a new instance is created and the exception class is missed
    }
    delete db;
    delete user;
    return "{ \"result\" : true }";
}


void UserManager::checkIfLoggedIn(std::string email, std::string token){
    rocksdb::DB* db = openDatabase("En check if logged in");

    try {
        User* user = User::load(db, email);
        user->checkToken(token);
    }catch(std::exception& e){
        delete db;
        throw; //Needs to be this way. If you throw e, a new instance is created and the exception class is missed
    }

    delete db;
}

void UserManager::addFileToUser(std::string email, int id) {
    rocksdb::DB* db = openDatabase("En add file to user");

    try {
        User* user = User::load(db, email);
        user->addSharedFile(id);
        user->save(db);
    }catch(std::exception& e){
        delete db;
        throw; //Needs to be this way. If you throw e, a new instance is created and the exception class is missed
    }

    delete db;
}

void UserManager::addFileToUserAsOwner(std::string email, int id) {
    rocksdb::DB* db = openDatabase("En add file to user");

    try {
        User* user = User::load(db, email);
        user->addFile(id);
        user->save(db);
    }catch(std::exception& e){
        delete db;
        throw; //Needs to be this way. If you throw e, a new instance is created and the exception class is missed
    }

    delete db;
}

std::string UserManager::loadUserFiles(std::string email) {
    rocksdb::DB* db = openDatabase("En load user files");
    User* user;
    try {
        user = User::load(db, email);
    }catch(std::exception& e){
        delete db;
        throw; //Needs to be this way. If you throw e, a new instance is created and the exception class is missed
    }
    delete db;

    FileManager f_manager;
    std::unordered_map<std::string, std::string> folders;
    for (struct userFile* file : user->getFiles()) {
        std::unordered_map<std::string,std::string>::const_iterator iterator = folders.find(file->path);
        if (iterator != folders.end()) {
            folders[file->path] += ", " + f_manager.loadFile(file->id);
        } else {
            folders[file->path] = f_manager.loadFile(file->id);
        }
    }

    std::string result = "{ \"result\" : true , \"files\" : [ ";
    int i = 0;
    for (auto& folder: folders) {
        if (i == 1) result += ", ";
        result += "{ \"folderPath\" : \"" + folder.first + "\" , \"files\" : [ " + folder.second + " ] }";
        i++;
    }
    result += " ] }";

    delete user;
    return result;
}

void UserManager::checkIfUserHasFilePermits(std::string email, int id) {
    rocksdb::DB* db = openDatabase("En load user files");

    User* user;
    try {
        user = User::load(db, email);
    }catch(std::exception& e){
        delete db;
        throw; //Needs to be this way. If you throw e, a new instance is created and the exception class is missed
    }
    delete db;

    if (!user->hasFile(id)) throw new HasNoPermits();

}

std::string UserManager::eraseFileFromUser(std::string email, int id) {
    rocksdb::DB* db = openDatabase("En erase file from user");

    User* user;
    try {
        user = User::load(db, email);
        user->eraseFile(id);
        user->save(db);
        if (user->isOwnerOfFile(id)) {
            ; //Eliminarlo de los demás a los que se lo compartí.
        }
    }catch(std::exception& e){
        delete db;
        throw; //Needs to be this way. If you throw e, a new instance is created and the exception class is missed
    }
    delete db;

    return "{ \"result\" : true }";
}