//
// Created by martin on 13/11/15.
//

#include <folder/folder.h>
#include <sys/stat.h>
#include "folderManager.h"
#include <stdio.h>

FolderManager::FolderManager() { }
FolderManager::~FolderManager() { }

std::string FolderManager::addFolder(std::string email, std::string path, std::string nameFolder){
    rocksdb::DB* db = this->openDatabase("En AddFolder: ",'w');

    Folder* folder = NULL;
    try {
        folder = Folder::load(db, email, path);
        folder->addFolder(nameFolder,db);
        folder->save(db);
    } catch (std::exception& e) {
        if (folder != NULL) delete folder;
        delete db;
        throw;
    }

    delete folder;
    delete db;

    mkdir(("files/"+email+"/path/"+nameFolder).c_str(), S_IRWXU | S_IRWXG | S_IRWXO);

    return "{ \"result\" : true }";
}

std::string FolderManager::renameFolder(std::string email, std::string path, std::string oldName, std::string newName){
    rocksdb::DB* db = this->openDatabase("En RenameFolder: ",'w');

    Folder* folder = NULL;
    try {
        folder = Folder::load(db, email, path);
        folder->renameFolder(oldName,newName);
        folder->save(db);
    } catch (std::exception& e) {
        if (folder != NULL) delete folder;
        delete db;
        throw;
    }

    try {
        folder = Folder::load(db, email, path+"/"+oldName);
        folder->renamePath(path+"/"+newName);
        folder->save(db);
    } catch (std::exception& e) {
        if (folder != NULL) delete folder;
        delete db;
        throw;
    }

    int result = rename( ("files/"+email+"/path/"+oldName).c_str(),("files/"+email+"/path/"+newName).c_str());

    delete folder;
    delete db;
    if ( result == 0 )
        return "{ \"result\" : true }";

    return "{ \"result\" : false }";
}