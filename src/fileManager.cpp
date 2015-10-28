//
// Created by luciano on 03/10/15.
//

#include "fileManager.h"
#include "userManager.h"


FileManager::FileManager() { }
FileManager::~FileManager() { }

std::string FileManager::saveFile(std::string email, std::string name, std::string extension, std::vector<std::string> tags){
    File* file = new File();
    file->setName(name);
    file->setExtension(extension);
    file->setOwner(email);
    file->setLastUser(email);
    for(std::string tag : tags ) {
        file->setTag(tag);
    }

    rocksdb::DB* db = this->openDatabase("En SaveFile: ");

    try {
        file->save(db);
    }catch(std::exception& e){
        delete db;
        throw; //Needs to be this way. If you throw e, a new instance is created and the exception class is missed
    }
    delete db;

    UserManager u_manager;
    u_manager.addFileToUserAsOwner(email, file->getMetadata()->id);

    delete file;

    return "{ \"result\" : \"true\" , \"fileID\" : " + std::to_string(file->getMetadata()->id) + " }";

}

std::string FileManager::saveNewVersionOfFile(std::string email, int id, std::string name, std::string extension, std::vector<std::string> tags){
    File* file = this->openFile(id);
    file->setName(name);
    file->setExtension(extension);
    file->setLastUser(email);
    for(std::string tag : tags ) {
        file->setTag(tag);
    }

    rocksdb::DB* db = this->openDatabase("En SaveNewVersionOfFile: ");

    try {
        file->save(db);
    }catch(std::exception& e){
        delete db;
        throw; //Needs to be this way. If you throw e, a new instance is created and the exception class is missed
    }
    delete db;

    UserManager u_manager;
    u_manager.addFileToUser(email, id);
    delete file;

    return "{ \"result\" : \"true\" }";

}

File* FileManager::openFile(int id) {
    File* file = new File();

    file->setId(id);

    rocksdb::DB* db = this->openDatabase("En LogIn: ");

    try{
        file->load(db);
    }catch(std::exception& e){
        delete db;
        throw;
    }
    delete db;
    return file;
}

std::string FileManager::loadFile(int id){

    File* file = this->openFile(id);

    Json::Value value;
    Json::StyledWriter writer;
    std::string json = writer.write(file->getJson());
    delete file;
    return json;

}

std::string FileManager::eraseFile(int id){

    File* file = new File();

    file->setId(id);

    rocksdb::DB* db = this->openDatabase("En LogIn: ");

    try {
        file->erase(db);
    }catch(std::exception& e){
        delete db;
        throw; //Needs to be this way. If you throw e, a new instance is created and the exception class is missed
    }
    //Should delete db inmediately after using it
    delete db;
    delete file;

    return "{ \"result\" : \"true\" , \"fileID\" : \" " + std::to_string(id) + "\" }";

}