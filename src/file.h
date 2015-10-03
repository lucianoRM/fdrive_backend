//
// Created by luciano on 02/10/15.
//

#ifndef TALLER_FILE_H
#define TALLER_FILE_H

#include <string>
#include <list>
#include <algorithm>
#include <ctime>
#include "rocksdb/db.h"
#include "json/json.h"
#include "json/json-forwards.h"
#include <iostream>
#include "errorManager.h"

struct metadata {

    int id;
    std::string name;
    std::string extension;
    std::string owner;
    struct tm* lastModified;
    std::string lastUser; //last user that modified the file
    std::list<std::string>* tags;

};

class File {

    private:
        struct metadata* metadata;

    private:
        bool notExists(rocksdb::DB* db); //Checks if the file is already in the db



    public:

        File();
        ~File();


        //Metadata setters
        void setName(std::string newName);
        void setExtension(std::string newExt);
        void setOwner(std::string newOwner);
        void setLastModDate( );
        void setLastUser(std::string newLastUser);
        void setTag(std::string newTag);

        struct metadata* getMetadata(); //Returns the metadata from the file.

        bool load(rocksdb::DB* db); //Loads the metadata from the db. Id needs to be set.
        bool save(rocksdb::DB* db); //Saves the metadata to the db
        bool erase(rocksdb::DB* db); //Erase the metadata from the db













};


#endif //TALLER_FILE_H
