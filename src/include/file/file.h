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
#include "exceptions/fileExceptions.h"
#include "exceptions/dbExceptions.h"
#include "exceptions/userExceptions.h"
#include "exceptions/requestExceptions.h"
#include "searchInformation/searchInformation.h"
#include "folder/folder.h"

// Struct containing the metadata belonging to a file.
struct metadata {
    int id;
    std::string name;
    std::string extension;
    std::string owner;
    std::string ownerPath; // Path of file in owner's account.
    std::string lastModified;
    std::string lastUser; // Last user that modified the file.
    std::list<std::string>* tags;
    int size; // In MB.

};

// Class to represent the logical structure of a File. In other wors its metadata,
// It's used to set the metadata whenever there is a change on a file, or there is a new File
// and also to get the information of the metadata.

class File {

    private:
        struct metadata* metadata;
        std::list<std::string>* users;

        bool notExists(rocksdb::DB* db); // Checks if the file is already in the db.
        std::string getKey(); // Returns key made from file metadata.
        void deleteFromUser(rocksdb::DB* db, std::string email, std::string path);

    public:
        /* In the File constructor:
         * lastModified is setted with the timestamp,
         * The id is setted in -1*/
        File();
        ~File();

        // Metadata setters.
        void setName(std::string newName);
        void setExtension(std::string newExt);
        void setOwner(std::string newOwner);
        void setOwnerPath(std::string newPath);
        void setLastModDate( );
        void setLastUser(std::string newLastUser);
        void setTag(std::string newTag);
        void setId(int id);
        void setSize(int size);
        void genId(rocksdb::DB* db); // ID to be generated when adding a new file.
        int getId();
        int getSize();
        std::list<std::string> getUsers();

        struct metadata* getMetadata(); // Returns the metadata from the file.
        Json::Value getJson(); // Returns Json value made from file metadata and users.

        static File* load(rocksdb::DB* db, int id);
        void load(rocksdb::DB* db); // Loads the metadata from the db. Id needs to be set.
        void save(rocksdb::DB* db); // Saves the metadata to the db
        //TODO save devuelve el número de la nueva versión.
        void saveSearches(std::string user, std::string path, rocksdb::DB* db); // Saves tag, name, extension and owner information for future searches done by the user.
        void changeSearchInformation(File* oldFile);

        void checkIfUserHasPermits(std::string email);

        void addSharedUser(std::string user);
        void eraseFromUser(rocksdb::DB* db, std::string email, std::string path);

};

#endif //TALLER_FILE_H
