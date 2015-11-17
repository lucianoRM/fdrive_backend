#ifndef FDRIVE_BACKEND_VERSION_H
#define FDRIVE_BACKEND_VERSION_H

#include <string.h>
#include <ctime>
#include <list>
#include <algorithm>
#include "json/json.h"
#include "json/json-forwards.h"
#include "exceptions/fileExceptions.h"

// Struct containing the metadata belonging to a file.
struct metadata {
    int size; // In MB.
    std::string name;
    std::string extension;
    std::string ownerPath; // Path of file in owner's account.
    std::string lastModified;
    std::string lastUser; // Last user that modified the file.
    std::list<std::string>* tags;
};

class Version {
    private:
        struct metadata *metadata;

        void setUpFromJson(Json::Value json);

    public:
        Version();
        ~Version();

        void setName(std::string newName);
        void setExtension(std::string newExt);
        void setOwnerPath(std::string newPath);
        void setLastModDate( );
        void setLastUser(std::string newLastUser);
        void setTag(std::string newTag);
        void setSize(int size);
        struct metadata* getMetadata();
        Json::Value getJson();
        int getSize();
        std::string getName();
        static Version* load(Json::Value json);

};

#endif //FDRIVE_BACKEND_VERSION_H
