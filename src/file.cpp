//
// Created by luciano on 02/10/15.
//

#include "file.h"
#include <iostream>

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

    this->metadata->name = newName;


}

void File::setExtension(std::string newExt){

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
    std::for_each(ftags->begin(), ftags->end(), [&newTag](std::string &n){if(n == newTag) return NULL; });

    this->metadata->tags->push_back(newTag);
}

struct metadata* File::getMetadata(){

    return this->metadata;


}


bool save(){ return true;} //Saves the metadata to the db
bool erase(){return true;} //Erase the metadata from the db
