#include "folder.h"
#include <exceptions/dbExceptions.h>
#include <exceptions/fileExceptions.h>
#include <exceptions/folderExceptions.h>

Folder::Folder() {
    this->filesIds = new std::vector<int>();
    this->filesNames = new std::vector<std::string>();
    this->folders = new std::vector<std::string>();
}

Folder::~Folder() {
    delete this->filesIds;
    delete this->filesNames;
    delete this->folders;
}

bool Folder::checkIfExisting(std::vector<std::string>* listToCheck, std::string value) {
    return (std::find(listToCheck->begin(), listToCheck->end(), value) != listToCheck->end());
}

bool Folder::checkIfExisting(std::vector<int>* listToCheck, int value) {
    return (std::find(listToCheck->begin(), listToCheck->end(), value) != listToCheck->end());
}

void Folder::addFolder(std::string folder) {
    if (checkIfExisting(this->folders, folder) ) {
        throw AlreadyExistentFolderException();
    }
    this->folders->push_back(folder);
}

void Folder::addFile(int fileId, std::string fileName) {
    if (this->checkIfExisting(this->filesIds, fileId)) {
        throw FileAlreadyInFolderException();
    }
    if (this->checkIfExisting(this->filesNames, fileName)) {
        throw FilenameTakenException();
    }

    this->filesIds->push_back(fileId);
    this->filesNames->push_back(fileName);
}

void Folder::removeFile(int fileId) {
    size_t i;
    for (i = 0; i < this->filesIds->size(); i++) {
        if (this->filesIds->at(i) == fileId) break;
    }
    if (i == this->filesIds->size()) throw FileNotInFolderException();
    this->filesIds->erase(this->filesIds->begin() + i);
    this->filesNames->erase(this->filesNames->begin() + i);
}

Json::Value Folder::getJson() {

    Json::Value root;

    Json::Value folders (Json::arrayValue);
    Json::Value files (Json::arrayValue);
    Json::Value filesNames (Json::arrayValue);


    for (std::string folder : *(this->folders)) {
        folders.append(folder);
    }
    root["folders"] = folders;

    for (int id : *(this->filesIds)) {
        files.append(id);
    }
    root["files"] = files;

    for (std::string fileName : *(this->filesNames)) {
        filesNames.append(fileName);
    }
    root["filesNames"] = filesNames;

    return root;
}


Folder* Folder::load(rocksdb::DB* db, std::string email, std::string path) {
    Folder* folder = new Folder();
    folder->user = email;
    folder->fullName = path;

    std::string value;
    rocksdb::Status status = db->Get(rocksdb::ReadOptions(),folder->user+"."+folder->fullName,&value);
    if (status.IsNotFound()) {
        delete folder;
        throw FolderNotFound();
    }

    // If here is because file exists in db.
    Json::Reader reader;
    Json::Value root;

    if (!reader.parse(value,root,false)) {
        delete folder;
        std::cout << "JsonCPP no pudo parsear en Folder::load. Value: " << value << ". root: " << root << std::endl;
        throw std::exception();
    }

    Json::Value folders = root["folders"];
    Json::Value files = root["files"];
    Json::Value filesNames = root["filesNames"];

    for (Json::Value::iterator it = folders.begin(); it != folders.end();it++) {
        folder->folders->push_back((*it).asString());
    }

    for (Json::Value::iterator it = files.begin(); it != files.end();it++) {
        folder->filesIds->push_back((*it).asInt());
    }

    for (Json::Value::iterator it = filesNames.begin(); it != filesNames.end();it++) {
        folder->filesNames->push_back((*it).asString());
    }

    return folder;
}

void Folder::save(rocksdb::DB* db) {
    Json::Value root = this->getJson();
    Json::StyledWriter writer;

    std::string json = writer.write(root);

    rocksdb::Status status = db->Put(rocksdb::WriteOptions(),user+"."+fullName,json);

    if (!status.ok()) throw DBException();
}

std::string Folder::getContent() {
    Json::Value json = getJson();
    Json::StyledWriter writer;
    return writer.write(json);
}