//
// Created by martin on 13/11/15.
//

#include <folder/folder.h>
#include <sys/stat.h>
#include "folderManager.h"
#include "folderExceptions.h"
#include <stdio.h>

FolderManager::FolderManager() { }
FolderManager::~FolderManager() { }

std::string FolderManager::addFolder(std::string email, std::string path, std::string nameFolder){
	if (path.compare("shared") == 0 || path.compare("trash") == 0) throw InvalidFolderPath();
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

std::vector<int> FolderManager::getFilesFromFolder(std::string email, std::string path) {
    rocksdb::DB* db = this->openDatabase("En getFilesFromFolder: ",'r');

    Folder* folder = NULL;
    std::vector<int> files;
    std::vector<std::string> folders ({path});
    while (!folders.empty()) {
        std::string actualPath = folders.back();
        folders.pop_back();
        try {
            folder = Folder::load(db, email, actualPath);
        } catch (std::exception& e) {   // Si el primero existe, todos los otros seguro existen.
            delete db;
            throw;
        }
        for (int id : folder->getDirectFiles()) {
            files.push_back(id);
        }
        for (std::string newFolder : folder->getDirectFolders()) {
            folders.push_back(actualPath + "/" + newFolder);
        }
        std::cout << std::endl;
        delete folder;
    }
    delete db;
    return files;
}

void FolderManager::renameFile(std::string oldName, std::string newName, std::string email, std::string path) {
    rocksdb::DB* db = this->openDatabase("En RenameFile: ",'w');
    Folder* folder = NULL;
    try {
        folder = Folder::load(db, email, path);
        folder->renameFile(oldName,newName);
        folder->save(db);
    } catch (std::exception& e) {
        if (folder != NULL) delete folder;
        delete db;
        throw;
    }
    delete folder;
    delete db;
}