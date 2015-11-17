#ifndef FDRIVE_FOLDER_H
#define FDRIVE_FOLDER_H

#include <string.h>
#include <vector>
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
        std::string user;  // Email of the user.
        std::string fullName;   // Path of the Folder.
        std::vector<std::string>* folders; // Folders inside this folder.
        std::vector<int>* filesIds;   // Files ids of the files inside this folder.
        std::vector<std::string>* filesNames;   // Files names of the files inside this folder.
        // List to check is either the folders list or the files list. And the value is corresponding to this
        // A folder path name or a file Id, respectively.
        bool checkIfExisting(std::vector<std::string>* listToCheck, std::string value);
        bool checkIfExisting(std::vector<int>* listToCheck, int value);
        Json::Value getJson();
        std::string getJsonFileStructure();


    public:
        Folder();
        ~Folder();

        void setName(std::string path);
        void setUser(std::string user);
        void addFolder(std::string folder, rocksdb::DB* db);
        void addFile(int fileId, std::string fileName);
        void renameFile(std::string oldName, std::string newName);
        void removeFile(int fileId);

        static Folder* load(rocksdb::DB* db, std::string email, std::string path);
        void save(rocksdb::DB* db);

        std::string getContent();
        void renameFolder(std::string oldName, std::string newName);
        void renamePath(std::string newName);
        void eraseFolder(std::string folder, rocksdb::DB* db);

        std::vector<int> getDirectFiles();
        std::vector<std::string> getDirectFolders();
};

#endif //FDRIVE_FOLDER_H
