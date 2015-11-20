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
        Json::Value _jsonFiles;
        if (!reader.parse(value, _jsonFiles, false)) {
            std::cout << "JsonCPP no pudo parsear en SearchInformation::load. Value: " << value << ". root: " << _jsonFiles << std::endl;
            delete info;
            throw std::exception();
        } else {
            for (Json::Value::iterator it = _jsonFiles["files"].begin(); it != _jsonFiles["files"].end(); it++) {
                struct searchFile* file = new searchFile;
                file->id = (*it)["id"].asInt();
                file->path = (*it)["path"].asString();
                info->files->push_back(file);
            }
        }
    }
    return info;
}

/* TODO Debería fijarse que si no tiene nada, no se guarde; y si existía, se borre.*/
void SearchInformation::save(rocksdb::DB* db) {
    if (!this->files->empty()) {
        Json::StyledWriter writer;
        rocksdb::Status status = db->Put(rocksdb::WriteOptions(),
                                         "search" + this->typeOfSearch + "." + this->user + "." + this->element,
                                         "{ \"files\" : " + writer.write(this->getJson()) + " }");
        if (!status.ok()) throw DBException();
    } else {
        rocksdb::Status status = db->Delete(rocksdb::WriteOptions(),
                                         "search" + this->typeOfSearch + "." + this->user + "." + this->element);
    }
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
    for (struct searchFile* file : *(this->files)) {
        if (file->id == id) {
            foundFile = file;
            break;
        }
    }
    if (foundFile == NULL) throw FileNotInSearch();
    this->files->remove(foundFile);
}

Json::Value SearchInformation::getJson() {
    Json::Value json(Json::arrayValue);
    for (struct searchFile* file : *(this->files)) {
        Json::Value newFile;
        newFile["id"] = file->id;
        newFile["path"] = file->path;
        json.append(newFile);
    }
    return json;
}

std::string SearchInformation::getContent() {
    Json::StyledWriter writer;
    return writer.write(this->getJson());
}

std::string SearchInformation::getUserPath(int id) {
    struct searchFile* foundFile = NULL;
    for (struct searchFile* file : *(this->files)) {
        if (file->id == id) {
            foundFile = file;
            break;
        }
    }
    if (foundFile == NULL) throw FileNotInSearch();
    return foundFile->path;
}