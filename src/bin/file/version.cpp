#include "file/version.h"

Version::Version() {
    this->metadata = new struct metadata;
    this->metadata->tags = new std::list<std::string>();
    this->setLastModDate();
}

Version::~Version() {
    delete this->metadata->tags;
    delete this->metadata;
}

void Version::setName(std::string newName) {
    if(newName == "~") throw FilenameNotValidException();
    this->metadata->name = newName;
}

void Version::setExtension(std::string newExt) {
    if(newExt == ".") throw FileExtensionNotValidException();
    this->metadata->extension = newExt;
}

void Version::setOwnerPath(std::string newPath) {
    this->metadata->ownerPath = newPath;
}

void Version::setLastModDate() {
    time_t currTime;
    time(&currTime);
    char* time = ctime(&currTime);
    time[strlen(time) - 1] = '\0'; // Removes \n at the end.
    this->metadata->lastModified = std::string(time);
}

void Version::setLastUser(std::string newLastUser) {
    this->metadata->lastUser = newLastUser;
}

void Version::setSize(int size) {
    this->metadata->size = size;
}

void Version::setTag(std::string newTag) {
    std::list<std::string>* ftags = this->metadata->tags;

    // Checks if the tag already exists.
    bool exists = false;
    std::for_each(ftags->begin(), ftags->end(), [&newTag,&exists](std::string &n){if(!n.compare(newTag)) {exists = true; return;} });

    if(!exists) this->metadata->tags->push_back(newTag);
}

struct metadata* Version::getMetadata() {
    return this->metadata;
}

Json::Value Version::getJson() {
    Json::Value root;
    root["name"] = this->metadata->name;
    root["extension"]  = this->metadata->extension;
    root["pathInOwner"] = this->metadata->ownerPath;
    root["lastModified"] = this->metadata->lastModified;
    root["lastUser"] = this->metadata->lastUser;
    root["size"] = this->metadata->size;
    Json::Value tags (Json::arrayValue);
    std::for_each(this->metadata->tags->begin(),this->metadata->tags->end(),[&tags](std::string &tag){tags.append(tag);});
    root["tags"] = tags;

    return root;
}

int Version::getSize() {
    return this->metadata->size;
}

void Version::setUpFromJson(Json::Value json) {
    this->metadata->size = json["size"].asInt();
    this->metadata->name = json["name"].asString();
    this->metadata->extension = json["extension"].asString();
    this->metadata->ownerPath = json["pathInOwner"].asString();
    this->metadata->lastUser = json["lastUser"].asString();
    this->metadata->lastModified = json["lastModified"].asString();
    Json::Value tags = json["tags"];
    for (Json::Value::iterator it = tags.begin(); it != tags.end(); it++) {
        this->metadata->tags->push_back((*it).asString());
    }
}

Version* Version::load(Json::Value json) {
    Version* version = new Version();
    version->setUpFromJson(json);
    return version;
}