//
// Created by martin on 1/11/15.
//
#include "folder.h"
#include <exceptions/dbExceptions.h>
#include <exceptions/fileExceptions.h>
#include <exceptions/folderExceptions.h>

Folder::Folder() {
    this->filesIds = new std::list<std::string>;
    this->filesNames = new std::list<std::string>;
    this->folders = new std::list<std::string>;
}

Folder::~Folder() {
    delete this->filesIds;
    delete this->filesNames;
    delete this->folders;
}

bool Folder::checkIfExisting(std::list<std::string>* listToCheck, std::string value) {
    return (std::find(listToCheck->begin(), listToCheck->end(), value) != listToCheck->end());
}

void Folder::addFolder(std::string folder) {
    if (checkIfExisting(this->folders, folder) ) {
        throw AlreadyExistentFolderException();
    }
    this->folders->push_back(folder);
}

void Folder::addFile(std::string fileId, std::string fileName) {
    if (checkIfExisting(this->filesNames, fileName)) {
        throw FilenameTakenException();
    }

    this->filesIds->push_back(fileId);
    this->filesNames->push_back(fileName);
}

Json::Value Folder::getJson() {
    Json::Value root;

    Json::Value folders;
    Json::Value files;
    Json::Value filesNames;

    std::for_each(this->folders->begin(),this->folders->end(),[&folders](std::string &folder){folders.append(folder);});
    root["folders"] = folders;

    std::for_each(this->filesIds->begin(),this->filesIds->end(),[&files](std::string &file){files.append(file);});
    root["files"] = files;

    std::for_each(this->filesNames->begin(),this->filesNames->end(),[&filesNames](std::string &file){filesNames.append(file);});
    root["filesNames"] = filesNames;

    return root;
}


void Folder::load(rocksdb::DB* db, std::string email, std::string path) {
    std::string value;

    this->email = email;
    this->name = path;

    rocksdb::Status status = db->Get(rocksdb::ReadOptions(),this->email+"."+this->name,&value);
    if (status.IsNotFound()) throw;

    // If here is because file exists in db.
    Json::Reader reader;
    Json::Value root;

    if (! reader.parse(value,root,false)) throw;

    Json::Value folders = root["folders"];
    Json::Value files = root["files"];
    Json::Value filesNames = root["filesNames"];

    for (Json::Value::iterator it = folders.begin(); it != folders.end();it++) {
        this->folders->push_back((*it).asString());
    }

    for (Json::Value::iterator it = files.begin(); it != files.end();it++) {
        this->filesIds->push_back((*it).asString());
    }

    for (Json::Value::iterator it = filesNames.begin(); it != filesNames.end();it++) {
        this->filesNames->push_back((*it).asString());
    }
}

void Folder::save(rocksdb::DB* db) {
    Json::Value root = this->getJson();
    Json::StyledWriter writer;

    std::string json = writer.write(root);

    rocksdb::Status status = db->Put(rocksdb::WriteOptions(),email+"."+name,json);

    if (! status.ok()) throw DBException();
}