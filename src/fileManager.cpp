//
// Created by luciano on 03/10/15.
//

#include "fileManager.h"


FileManager::FileManager() { }
FileManager::~FileManager() { }

void FileManager::saveFile(struct mg_connection* conn){

    //Needed for filtering unnecesary headers
    char json[conn->content_len+1];
    char* content = conn->content;
    content[conn->content_len] = '\0';
    strcpy(json,conn->content);

    Json::Reader reader;
    Json::Value root;

    if(!reader.parse(json,root,false)){
        throw FileException();
    }

    std::string name,extension,owner;

    //Checks if the keys exists
    if(!root.isMember("name") || !root.isMember("extension") || !root.isMember("owner") || !root.isMember("tags"))
        throw RequestException();


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

    try {
        file->save(db);
    }catch(std::exception& e){
        delete db;
        throw; //Needs to be this way. If you throw e, a new instance is created and the exception class is missed
    }

    //Should delete db inmediately after using it
    delete db;

    mg_printf_data(conn,"File ID: %d",file->getMetadata()->id);

    delete file;

}

void FileManager::loadFile(struct mg_connection* conn){


    char id[100];
    // Get form variables
    mg_get_var(conn, "id", id, sizeof(id));

    File* file = new File();

    file->setId(atoi(id));

    rocksdb::DB* db = this->openDatabase("En LogIn: ");

    try{
        file->load(db);
    }catch(std::exception& e){
        delete db;
        throw;//Needs to be this way. If you throw e, a new instance is created and the exception class is missed
    }
    //Should delete db inmediately after using it
    delete db;

    Json::Value value;
    Json::StyledWriter writer;

    std::string json = writer.write(file->getJson());

    mg_printf_data(conn, "%s", json.c_str());

    delete file;



}

void FileManager::eraseFile(struct mg_connection* conn){

    //Needed for filtering unnecesary headers
    char json[conn->content_len+1];
    char* content = conn->content;
    content[conn->content_len] = '\0';
    strcpy(json,conn->content);

    Json::Reader reader;
    Json::Value root;

    if(!reader.parse(json,root,false)){
        throw FileException();
    }

    int id;

    //Checks if the keys exists
    if(!root.isMember("id")) throw RequestException();

    id = root["id"].asInt();

    File* file = new File();

    file->setId(id);

    rocksdb::DB* db = this->openDatabase("En LogIn: ");

    try {
        file->erase(db);
    }catch(std::exception& e){
        delete db;
        throw; //Needs to be this way. If you throw e, a new instance is created and the exception class is missed
    }
    //Should delete db inmediately after using it
    delete db;

    mg_printf_data(conn, "File Erased, id= %d", id);

    delete file;


}