//
// Created by martin on 1/11/15.
//

#ifndef FDRIVE_FOLDER_H
#define FDRIVE_FOLDER_H

#include <string.h>
#include <list>
#include <algorithm>
#include <ctime>
#include "rocksdb/db.h"
#include "json/json.h"
#include "json/json-forwards.h"
#include <iostream>
#include "dbExceptions.h"

// Class to represent the logical structure of a Folder.

class Folder {

    private:
        std::string email;  // Email of the user.
        std::string name;   // Path of the Folder.
        std::list<std::string>* folders; // Folders inside this folder.
        std::list<std::string>* filesIds;   // Files ids of the files inside this folder.
        std::list<std::string>* filesNames;   // Files names of the files inside this folder.
        // List to check is either the folders list or the files list. And the value is corresponding to this
        // A folder path name or a file Id, respectively.
        bool checkIfExisting(std::list<std::string>* listToCheck, std::string value);

    public:
        Folder();
        ~Folder();
        Json::Value getJson();
        void addFolder(std::string folder);
        void addFile(std::string fileId, std::string fileName);
        void load(rocksdb::DB* db, std::string email, std::string path);
        void save(rocksdb::DB* db);
};
#endif //FDRIVE_FOLDER_H
