//
// Created by martin on 13/11/15.
//

#include <folder/folder.h>
#include "folderManager.h"

FolderManager::FolderManager() { }
FolderManager::~FolderManager() { }

std::string FolderManager::addFolder(std::string email, std::string path, std::string nameFolder){
    rocksdb::DB* db = this->openDatabase("En AddFolder: ",'w');

    try {
        Folder *folder = Folder::load(db, email, path);
        folder->addFolder(nameFolder);
        folder->save(db);
    } catch (std::exception& e) {
        if (folder != NULL) delete folder;
        delete db;
        throw;
    }

    delete folder;
    delete db;


    return "{ \"result\" : true }";
}