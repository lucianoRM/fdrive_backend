//
// Created by luciano on 02/10/15.
//

#include "file.h"

File::File(){

    this->metadata = new struct metadata;
    this->metadata->id = -1;
    this->metadata->extension = ".";
    this->metadata->name = "";
    time_t currTime;
    time(&currTime);
    this->metadata->lastModified = gmtime(&currTime);
    this->metadata->lastUser = "";
    this->metadata->owner = "";
    this->metadata->tags = new std::list<std::string>;
}

File::~File(){

    delete this->metadata->tags;
    delete this->metadata;

}



void File::setName(std::string newName){

    if(newName == "~") throw errorCode::NAME_NOT_VALID;
    this->metadata->name = newName;


}

void File::setExtension(std::string newExt){

    if(newExt == ".") throw errorCode::EXTENSION_NOT_VALID;
    this->metadata->extension = newExt;


}

void File::setOwner(std::string newOwner){

    this->metadata->owner = newOwner;

}

void File::setLastModDate(){

    time_t currTime;
    time(&currTime);
    this->metadata->lastModified = gmtime(&currTime);


}

void File::setLastUser(std::string newLastUser){

    this->metadata->lastUser = newLastUser;
}

void File::setTag(std::string newTag){

    std::list<std::string>* ftags = this->metadata->tags;
    //Checks if the tag already exists
    std::for_each(ftags->begin(), ftags->end(), [&newTag](std::string &n){if(n == newTag) return; });

    this->metadata->tags->push_back(newTag);
}

struct metadata* File::getMetadata(){

    return this->metadata;


}

std::string File::getKey() {

    std::string key = this->metadata->name + this->metadata->extension + ":" + this->metadata->owner;
    return key;

}


bool File::notExists(rocksdb::DB* db){

    std::string fileKey = this->getKey();
    std::string value;
    rocksdb::Status status = db->Get(rocksdb::ReadOptions(), "files.keys."+std::string(fileKey), &value);
    if (status.IsNotFound()) return true;
    return false;


}

bool File::load(rocksdb::DB* db){ return true;} //Loads metadata from the db, id should be set.


bool File::save(rocksdb::DB* db){

    int fileId = this->metadata->id;

    if(fileId < 0) { //if id is not set and file doesn't exists

        if(!this->notExists(db)) throw errorCode::NAME_NOT_VALID; //File already exists and id not set. Trying to upload a new file with taken key. Not possible

        //Checks if it is the first file, has to initialize id counter.
        std::string value;
        rocksdb::Status status = db->Get(rocksdb::ReadOptions(), "files.maxID", &value);

        //if not found has to initialize it
        if (status.IsNotFound()) {
            rocksdb::Status status = db->Put(rocksdb::WriteOptions(), "files.maxID", "0");
            if (!status.ok()) return false;
            fileId = 0;
        }else {
            //If here is because id counter was initialized
            int maxID = stoi(value);
            rocksdb::Status status = db->Put(rocksdb::WriteOptions(), "files.maxID", std::to_string(maxID+1));
            if (!status.ok()) return false;
            fileId = maxID + 1;
        }
        this->metadata->id = fileId;
    }


    //Saves file into id hash
    std::string fileKey = this->getKey();
    rocksdb::Status status = db->Put(rocksdb::WriteOptions(),"files.keys."+std::string(fileKey),std::to_string(fileId));
    if(!status.ok()) return false;


    status = db->Put(rocksdb::WriteOptions(),"files."+std::to_string(fileId),this->metadata->name);


    return (status.ok());


}
bool File::erase(rocksdb::DB* db){return true;} //Erase the metadata from the db
