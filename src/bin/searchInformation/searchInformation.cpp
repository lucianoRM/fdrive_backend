#include "searchInformation.h"

SearchInformation::SearchInformation() {
    files = new std::list<struct searchFile*>();
}

SearchInformation::~SearchInformation() {
    for (struct searchFile* file : *files) {
        delete file;
    }
    delete files;
}

SearchInformation* SearchInformation::load(rocksdb::DB* db, std::string typeOfSearch, std::string user, std::string element) {
    SearchInformation* info = new SearchInformation();
    info->typeOfSearch = typeOfSearch;
    info->user = user;
    info->element = element;

    std::string value;
    rocksdb::Status status = db->Get(rocksdb::ReadOptions(), "search" + typeOfSearch + "." + user + "." + element, &value);
    if (!status.IsNotFound()) {
        Json::Reader reader;
        Json::Value jsonFiles;
        if (!reader.parse(value, jsonFiles, false)) {
            std::cout << "JsonCPP no pudo parsear en SearchInformation::load. Value: " << value << ". root: " << jsonFiles << std::endl;
            delete info;
            throw std::exception();
        } else {
            for (Json::Value::iterator it = jsonFiles["files"].begin(); it != jsonFiles["files"].end(); it++) {
                struct searchFile* file = new searchFile;
                file->id = (*it).asInt();
                file->path = (*it).asString();
                info->files->push_back(file);
            }
        }
    }
    return info;
}

/* TODO Debería fijarse que si no tiene nada, no se guarde; y si existía, se borre.*/
void SearchInformation::save(rocksdb::DB* db) {
    Json::Value json;
    for (struct searchFile* file : *files) {
        Json::Value newFile;
        newFile["id"] = file->id;
        newFile["path"] = file->path;
        json.append(newFile);
    }
    Json::StyledWriter writer;
    rocksdb::Status status = db->Put(rocksdb::WriteOptions(), "search" + this->typeOfSearch + "." + this->user + "." + this->element, "{ \"files\" : " + writer.write(json) + " }" );
    if (!status.ok()) throw DBException();
}

void SearchInformation::addFile(int id, std::string path) {
    struct searchFile* file = new searchFile;
    file->id = id;
    file->path = path;
    this->files->push_back(file);
}

/* Supone que no hay copias de un mismo archivo en distintos paths.*/
void SearchInformation::eraseFile(int id) {
    struct searchFile* foundFile = NULL;
    for (struct searchFile* file : *files) {
        if (file->id == id) {
            foundFile = file;
            break;
        }
    }
    if (foundFile == NULL) throw FileToEraseNotInSearch();
    this->files->remove(foundFile);
}