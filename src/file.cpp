//
// Created by luciano on 02/10/15.
//

#include "file.h"

File::File(){

    metadata = new struct metadata;

    metadata->id = 0;
    metadata->extension = ".";
    metadata->name = "";
   // metadata->lastModified = "";
    metadata->lastUser = "";
    metadata->owner = "";
    metadata->tags = new std::list<std::string>;
}

File::~File(){

    delete metadata->tags;
    delete metadata;

}



bool File::setName(std::string newName){

    this->metadata->name = newName;
    return true;


}

bool File::setExtension(std::string newExt){

    this->metadata->extension = newExt;
    return true;

}

bool File::setOwner(std::string newOwner){

    this->metadata->owner = newOwner;
    return true;

}

/*bool File::setLastModDate(std::string newDate){

    this->metadata->


}*/

bool File::setLastUser(std::string newLastUser){

    this->metadata->lastUser = newLastUser;
    return true;


}

bool File::setTag(std::string newTag){

    std::list<std::string>* ftags = this->metadata->tags;
    //Checks if the tag already exists
    std::for_each(ftags->begin(), ftags->end(), [&newTag](std::string &n){if(n == newTag) return false; });
    this->metadata->tags->push_back(newTag);
    return true;
}


bool save(){ return true;} //Saves the metadata to the db
bool erase(){return true;} //Erase the metadata from the db
