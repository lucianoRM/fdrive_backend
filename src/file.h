//
// Created by luciano on 02/10/15.
//

#ifndef TALLER_FILE_H
#define TALLER_FILE_H

#include <string>
#include <list>
#include <algorithm>

struct metadata {

    int id;
    std::string name;
    std::string extension;
    std::string owner;
    std::string lastModified;
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
        bool setName(std::string newName);
        bool setExtension(std::string newExt);
        bool setOwner(std::string newOwner);
        bool setLastModDate(std::string newDate);
        bool setLastUser(std::string newLastUser);
        bool setTag(std::string newTag);


        bool save(); //Saves the metadata to the db
        bool erase(); //Erase the metadata from the db













};


#endif //TALLER_FILE_H
