//
// Created by luciano on 02/10/15.
//

#ifndef TALLER_FILE_H
#define TALLER_FILE_H

#include <string.h>
#include <list>
#include <algorithm>
#include <ctime>
#include "rocksdb/db.h"
#include "json/json.h"
#include "json/json-forwards.h"
#include <iostream>
#include "fileExceptions.h"
#include "dbExceptions.h"
#include "requestExceptions.h"

struct metadata {

    int id;
    std::string name;
    std::string extension;
    std::string owner;
    std::string lastModified;
    std::string lastUser; // last user that modified the file.
    std::list<std::string>* tags;

};

class File {

    private:
        struct metadata* metadata;

    private:
        bool notExists(rocksdb::DB* db); //Checks if the file is already in the db
        std::string getKey(); //Returns key made from file metadata
        bool genId(rocksdb::DB* db); //If file wanted to be saved is new, a new id is generated.

    public:
        /* Al crear el archivo, se setea el lastModified con el timestamp,
         * el id a -1*/
        File();
        ~File();


        //Metadata setters
        void setName(std::string newName);
        void setExtension(std::string newExt);
        void setOwner(std::string newOwner);
        void setLastModDate( );
        void setLastUser(std::string newLastUser);
        void setTag(std::string newTag);
        void setId(int id);

        struct metadata* getMetadata(); //Returns the metadata from the file.
        Json::Value getJson(); //Returns Json value made from file metadata

        void load(rocksdb::DB* db); //Loads the metadata from the db. Id needs to be set.
        void save(rocksdb::DB* db); //Saves the metadata to the db

};


#endif //TALLER_FILE_H
