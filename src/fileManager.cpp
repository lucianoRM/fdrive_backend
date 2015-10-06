//
// Created by luciano on 03/10/15.
//

#include "fileManager.h"


FileManager::FileManager() { }
FileManager::~FileManager() { }

bool FileManager::saveFile(struct mg_connection* conn){

    //Needed for filtering unnecesary headers
    char json[conn->content_len+1];
    char* content = conn->content;
    content[conn->content_len] = '\0';
    strcpy(json,conn->content);

    Json::Reader reader;
    Json::Value root;

    reader.parse(json,root,false);

    std::string name,extension,owner;

    name = root["name"].asString();
    extension = root["extension"].asString();
    owner = root["owner"].asString();
    Json::Value tags = root["tags"];


    File* file = new File();

    file->setName(std::string(name));
    file->setExtension(std::string(extension));
    file->setOwner(std::string(owner));
    for( Json::ValueIterator itr = tags.begin() ; itr != tags.end() ; itr++ ) {
        file->setTag((*itr).asString());
    }

    rocksdb::DB* db = this->openDatabase("En LogIn: ");
    if (!db) {
        return 1;
    }

    bool result = file->save(db);

    //Should delete db inmediately after using it
    delete db;

    mg_printf_data(conn, "{ \"result\":  \"%s\" }, File ID: %d", result ? "true" : "false",file->getMetadata()->id);

    delete file;

    return true;
}

bool FileManager::loadFile(struct mg_connection* conn){

    char id[100];
    // Get form variables
    mg_get_var(conn, "id", id, sizeof(id));

    File* file = new File();

    file->setId(atoi(id));

    rocksdb::DB* db = this->openDatabase("En LogIn: ");
    if (!db) {
        return 1;
    }

    bool result = file->load(db);

    //Should delete db inmediately after using it
    delete db;

    Json::Value value;
    Json::StyledWriter writer;

    std::string json = writer.write(file->getJson());

    mg_printf_data(conn, "%s", json.c_str());

    delete file;

    return true;



}