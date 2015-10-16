//
// Created by luciano on 03/10/15.
//

#include "fileManager.h"
#include "userManager.h"


FileManager::FileManager() { }
FileManager::~FileManager() { }

std::string FileManager::saveFile(std::string name, std::string extension, std::string owner, std::vector<std::string> tags){
    File* file = new File();
    file->setName(name);
    file->setExtension(extension);
    file->setOwner(owner);
    for(std::string tag : tags ) {
        file->setTag(tag);
    }

    rocksdb::DB* db = this->openDatabase("En LogIn: ");

    try {
        file->save(db);
    }catch(std::exception& e){
        delete db;
        throw; //Needs to be this way. If you throw e, a new instance is created and the exception class is missed
    }
    delete db;

    int id = file->getMetadata()->id;
    UserManager u_manager;
    u_manager.addFileToUser(owner,id);

    delete file;

    return "{ \"result\" : \"true\" , \"fileID\" : " + std::to_string(file->getMetadata()->id) + " }";

}

std::string FileManager::loadFile(int id){

    File* file = new File();

    file->setId(id);

    rocksdb::DB* db = this->openDatabase("En LogIn: ");

    try{
        file->load(db);
    }catch(std::exception& e){
        delete db;
        throw;//Needs to be this way. If you throw e, a new instance is created and the exception class is missed
    }
    //Should delete db inmediately after using it
    delete db;

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