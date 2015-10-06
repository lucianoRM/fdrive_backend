//
// Created by luciano on 03/10/15.
//

#include "fileManager.h"


FileManager::FileManager() { }
FileManager::~FileManager() { }

bool FileManager::addFile(struct mg_connection* conn){

    //TODO:Should be json.
    char name[100], extension[100], owner[100];
    // Get form variables
    mg_get_var(conn, "name", name, sizeof(name));
    mg_get_var(conn, "extension", extension, sizeof(extension));
    mg_get_var(conn, "owner", owner,sizeof(owner));

    File* file = new File();

    file->setName(std::string(name));
    file->setExtension(std::string(extension));
    file->setOwner(std::string(owner));

    rocksdb::DB* db = this->openDatabase("En LogIn: ");
    if (!db) {
        return 1;
    }

    bool result = file->save(db);

    //Should delete db inmediately after using it
    delete db;

    mg_printf_data(conn, "{ \"result\":  \"%s\" }, File ID: %d", result ? "true" : "false",file->getMetadata()->id);

    delete file;

    return true;
}

bool FileManager::loadFile(struct mg_connection* conn){

    //TODO:Should be json.
    char id[100];
    // Get form variables
    mg_get_var(conn, "id", id, sizeof(id));

    File* file = new File();

    file->setId(atoi(id));

    std::cout << atoi(id) << std::endl;

    rocksdb::DB* db = this->openDatabase("En LogIn: ");
    if (!db) {
        return 1;
    }

    bool result = file->load(db);

    //Should delete db inmediately after using it
    delete db;

    mg_printf_data(conn, "{ \"result\":  \"%s\" }, name: %s\nowner: %s\n,lastMod: %s\n,lastOwner: %s\n", result ? "true" : "false",file->getMetadata()->name.c_str(),file->getMetadata()->owner.c_str(),file->getMetadata()->lastModified.c_str(),file->getMetadata()->lastUser.c_str()); 

    delete file;

    return true;



}