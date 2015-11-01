//
// Created by martin on 1/11/15.
//

#ifndef FDRIVE_FOLDER_H
#define FDRIVE_FOLDER_H

#include <string.h>
#include <list>
#include <rocksdb/db.h>

// Class to represent the logical structure of a Folder.

class Folder {

    private:
        std::string email;  // Email of the user.
        std::string name;   // Path of the Folder.
        std::list<std::string> folders; // Folders inside this folder.
        std::list<std::string> files;   // Files inside this folder.

    public:
        Folder();
        ~Folder();

        void addFolder(std::string email, std::string path, std::string folder);
        void addFile(std::string email, std::string path, std::string file);

        void load(rocksdb::DB* db, std::string email, std::string path);
        void save(rocksdb::DB* db);
};
#endif //FDRIVE_FOLDER_H
