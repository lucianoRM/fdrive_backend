//
// Created by luciano on 02/10/15.
//

#ifndef TALLER_FILE_H
#define TALLER_FILE_H

#include <string>
#include <list>
#include <algorithm>
#include <ctime>

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

        bool save(); //Saves the metadata to the db
        bool erase(); //Erase the metadata from the db













};


#endif //TALLER_FILE_H
