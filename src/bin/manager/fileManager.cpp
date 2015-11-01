//
// Created by luciano on 03/10/15.
//

#include <folder/folder.h>
#include "fileManager.h"
#include "userManager.h"

FileManager::FileManager() { }
FileManager::~FileManager() { }

std::string FileManager::saveFile(std::string email, std::string name, std::string extension, std::string path, std::vector<std::string> tags) {
    rocksdb::DB* db = this->openDatabase("En SaveFile: ");

    File* file = new File();
    file->setName(name);
    file->setExtension(extension);
    file->setOwner(email);
    file->setLastUser(email);
    for (std::string tag : tags ) {
        file->setTag(tag);
    }
    file->genId(db);
    std::string idFile = file->getId();

    Folder* folder = new Folder();
    folder->load(db,email,path);
    folder->addFile(email,path,idFile);
    folder->save(db);

    UserManager u_manager;
    /* TODO check there is no same name in the same path.
     * If there is: throw FilenameTakenException(); */

    try {
        file->save(db);
    } catch (std::exception& e) {
        delete db;
        throw; // Needs to be this way. If you throw e, a new instance is created and the exception class is missed.
    }


    delete db;

    u_manager.addFileToUserAsOwner(email, file->getMetadata()->id, path);

    int fileID = file->getMetadata()->id;

    delete file;

    return "{ \"result\" : true , \"fileID\" : " + std::to_string(fileID) + " }";
}

std::string FileManager::saveNewVersionOfFile(std::string email, int id, std::string name, std::string extension, std::vector<std::string> tags) {
    File* file = this->openFile(id);
    file->setName(name);
    file->setExtension(extension);
    file->setLastUser(email);
    for (std::string tag : tags ) {
        file->setTag(tag);
    }

    rocksdb::DB* db = this->openDatabase("En SaveNewVersionOfFile: ");

    try {
        file->save(db);
    } catch(std::exception& e) {
        delete db;
        throw; // Needs to be this way. If you throw e, a new instance is created and the exception class is missed,
    }
    delete db;
    delete file;

    return "{ \"result\" : \"true\" }";
}

File* FileManager::openFile(int id) {
    File* file = new File();

    file->setId(id);

    rocksdb::DB* db = this->openDatabase("En OpenFile: ");

    try {
        file->load(db);
    } catch(std::exception& e) {
        delete db;
        throw;
    }
    delete db;
    return file;
}

std::string FileManager::loadFile(int id) {
    File* file = this->openFile(id);

    Json::Value value;
    Json::StyledWriter writer;
    std::string json = writer.write(file->getJson());
    delete file;
    return json;
}