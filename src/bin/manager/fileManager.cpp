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
    file->setOwnerPath(path);
    file->setLastUser(email);
    for (std::string tag : tags ) {
        file->setTag(tag);
    }

    Folder* folder = NULL;
    // To check if there is a existing file with the same name File in current path.
    try {
        file->genId(db);
        folder = Folder::load(db, email, path);
    } catch (std::exception& e) {
        delete file;
        if (folder != NULL) delete folder;
        delete db;
        throw;
    }

    int fileID = file->getId();

    try {
        folder->addFile(fileID, name+extension);
        folder->save(db);
        file->save(db);
        file->saveSearches(email, path, db);
    } catch (std::exception& e) {
        delete file;
        delete folder;
        delete db;
        throw;
    }

    delete file;
    delete folder;

    return "{ \"result\" : true , \"fileID\" : " + std::to_string(fileID) + " }";
}

std::string FileManager::saveNewVersionOfFile(std::string email, int id, std::string name, std::string extension, std::vector<std::string> tags) {
    File* file = this->openFile(id);
    File* oldFile;
    try {
        oldFile = this->openFile(id);
    } catch(std::exception& e) {
        delete file;
        throw;
    }
    file->setName(name);
    file->setExtension(extension);
    file->setLastUser(email);
    for (std::string tag : tags ) {
        file->setTag(tag);
    }

    rocksdb::DB* db = this->openDatabase("En SaveNewVersionOfFile: ");
    try {
        file->changeSearchInformation(oldFile);
        file->save(db);
    } catch(std::exception& e) {
        delete oldFile;
        delete file;
        delete db;
        throw; // Needs to be this way. If you throw e, a new instance is created and the exception class is missed,
    }
    delete db;
    delete file;

    return "{ \"result\" : \"true\" }"; //TODO devuelve número de versión nueva.
}

File* FileManager::openFile(int id) {
    File* file = new File();

    file->setId(id);

    rocksdb::DB* db = NULL;

    try {
        db = this->openDatabase("En OpenFile: ");
        file->load(db);
    } catch(std::exception& e) {
        delete file;
        if (db != NULL) delete db;
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

void FileManager::checkIfUserHasFilePermits(int id, std::string email) {
    File* file = this->openFile(id);
    try {
        file->checkIfUserHasPermits(email);
    } catch(std::exception& e) {
        delete file;
        throw;
    }
    delete file;
}

std::string FileManager::shareFileToUser(int id, std::string email) {
    File* file = this->openFile(id);
    rocksdb::DB *db = NULL;
    Folder* folder = NULL;
    try {
        db = this->openDatabase("En checkIfUserHasFilePermits: ");
        file->addSharedUser(email);
        file->save(db);
        folder = Folder::load(db, email, "shared");
        folder->addFile(id, file->getMetadata()->name + file->getMetadata()->extension);
        folder->save(db);
        file->saveSearches(email, "shared", db);
    } catch (std::exception& e) {
        if (db != NULL) delete db;
        if (folder != NULL) delete folder;
        delete file;
        throw;
    }
    return "{ \"result\" : true }";
}


std::string FileManager::eraseFileFromUser(int id, std::string email, std::string path) {
    File* file = this->openFile(id);
    rocksdb::DB *db = NULL;
    try {
        db = this->openDatabase("En eraseFileFromUser: ");
        file->eraseFromUser(db, email, path);
    } catch (std::exception& e) {
        if (db != NULL) delete db;
        delete file;
        throw;
    }
    return "{ \"result\" : true }";
}