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

    try {
        file->setName(std::string(name));
        file->setExtension(std::string(extension));
        file->setOwner(std::string(owner));
    }catch(errorCode e){
        ErrorManager* em = new ErrorManager();
        mg_printf_data(conn, "%s",em->getMessage(e).c_str());
        delete em;
        return false;
    }

    rocksdb::DB* db;
    rocksdb::Options options;
    options.create_if_missing = true;
    rocksdb::Status status = rocksdb::DB::Open(options, "testdb", &db);
    if (!status.ok()){ std::cout << "En AddUser:" << status.ToString() << std::endl; }

    bool result;

    try{
        result = file->save(db);
    }catch (errorCode e){
        ErrorManager* em = new ErrorManager();
        mg_printf_data(conn, "%s",em->getMessage(e).c_str());
        delete em;
        delete db;
        return false;
    }


    delete db;

    mg_printf_data(conn, "{ \"result\":  \"%s\" }, File ID: %d", result ? "true" : "false",file->getMetadata()->id);

    delete file;


    return true;
}

bool FileManager::loadFile(struct mg_connection* conn){return true;}