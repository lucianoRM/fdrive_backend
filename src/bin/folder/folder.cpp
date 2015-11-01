//
// Created by martin on 1/11/15.
//

#include <exceptions/dbExceptions.h>
#include "folder.h"

Folder::Folder() {

    this->files = new std::list<std::string>;
    this->folders = new std::list<std::string>;
}

Folder::~Folder() {
    delete this->files;
    delete this->folders;
}

void Folder::addFolder(std::string email, std::string path, std::string folder) {
    this->email = email;
    this->name = path;
    this->folders->push_back(folder);
}

void Folder::addFile(std::string email, std::string path, std::string file) {
    this->email = email;
    this->name = path;
    this->files->push_back(file);
}

Json::Value Folder::getJson() {
    Json::Value root;

    Json::Value folders;
    Json::Value files;

    std::for_each(this->folders->begin(),this->folders->end(),[&folders](std::string &folder){folders.append(folder);});
    root["folders"] = folders;

    std::for_each(this->files->begin(),this->files->end(),[&files](std::string &file){files.append(file);});
    root["files"] = files;

    return root;
}


void Folder::load(rocksdb::DB* db, std::string email, std::string path) {
    std::string value;
    rocksdb::Status status = db->Get(rocksdb::ReadOptions(),"email."+path,&value);
    if (status.IsNotFound()) throw;

    // If here is because file exists in db.
    Json::Reader reader;
    Json::Value root;

    if (! reader.parse(value,root,false)) throw;

    // Load metadata into file.
    Json::Value folders = root["folders"];
    Json::Value files = root["files"];

    for (Json::Value::iterator it = folders.begin(); it != folders.end();it++) {
        this->folders->push_back((*it).asString());
    }

    for (Json::Value::iterator it = files.begin(); it != files.end();it++) {
        this->files->push_back((*it).asString());
    }
}

void Folder::save(rocksdb::DB* db) {
    Json::Value root = this->getJson();
    Json::StyledWriter writer;

    std::string json = writer.write(root);

    rocksdb::Status status = db->Put(rocksdb::WriteOptions(),"email."+name,json);

    if (! status.ok()) throw DBException();
}