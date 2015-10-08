//
// Created by luciano on 02/10/15.
//

#include "file.h"

File::File(){

    this->metadata = new struct metadata;
    this->metadata->id = -1;
    this->metadata->extension = ".";
    this->metadata->name = "";
    time_t currTime;
    time(&currTime);
    char* time = ctime(&currTime);
    time[strlen(time) - 1] = '\0'; //Removes \n at the end
    this->metadata->lastModified = std::string(time);
    this->metadata->lastUser = "";
    this->metadata->owner = "";
    this->metadata->tags = new std::list<std::string>;
}

File::~File(){

    delete this->metadata->tags;
    delete this->metadata;

}



void File::setName(std::string newName){

    if(newName == "~") throw FilenameNotValidException();
    this->metadata->name = newName;


}

void File::setExtension(std::string newExt){

    if(newExt == ".") throw FileExtensionNotValidException();
    this->metadata->extension = newExt;


}

void File::setOwner(std::string newOwner){

    this->metadata->owner = newOwner;

}

void File::setLastModDate(){

    time_t currTime;
    time(&currTime);
    char* time = ctime(&currTime);
    time[strlen(time) - 1] = '\0'; //Removes \n at the end
    this->metadata->lastModified = std::string(time);


}

void File::setLastUser(std::string newLastUser){

    this->metadata->lastUser = newLastUser;
}

void File::setTag(std::string newTag){

    std::list<std::string>* ftags = this->metadata->tags;
    //Checks if the tag already exists
    bool exists = false;
    std::for_each(ftags->begin(), ftags->end(), [&newTag,&exists](std::string &n){if(!n.compare(newTag)) {exists = true; return;} });

    if(!exists) this->metadata->tags->push_back(newTag);
}


void File::setId(int id){

    this->metadata->id = id;
}

struct metadata* File::getMetadata(){

    return this->metadata;


}

std::string File::getKey() {

    std::string key = this->metadata->name + this->metadata->extension + ":" + this->metadata->owner;
    return key;

}


Json::Value File::getJson(){

    Json::Value root;
    root["name"] = this->metadata->name;
    root["extension"]  = this->metadata->extension;
    root["owner"] = this->metadata->owner;
    root["id"] = this->metadata->id;
    root["lastModified"] = this->metadata->lastModified;
    root["lastUser"] = this->metadata->lastUser;

    Json::Value tags;
    std::for_each(this->metadata->tags->begin(),this->metadata->tags->end(),[&tags](std::string &tag){tags.append(tag);});
    root["tags"] = tags;

    return root;


}


bool File::notExists(rocksdb::DB* db){

    std::string fileKey = this->getKey();
    std::string value;
    rocksdb::Status status = db->Get(rocksdb::ReadOptions(), "files.keys."+std::string(fileKey), &value);
    if (status.IsNotFound()) return true;
    return false;


}


//If this function is called is because a file with no id(-1) is trying to be saved.
bool File::genId(rocksdb::DB* db){

    //Checks if the fileKey is in the db.
    if(!this->notExists(db)) throw FilenameTakenException(); //File already exists and id not set. Trying to upload a new file with taken key. Not possible


    //Gets id counter
    int fileId;
    std::string value;
    rocksdb::Status status = db->Get(rocksdb::ReadOptions(), "files.maxID", &value);


    //if not found has to initialize it
    if (status.IsNotFound()) {
        rocksdb::Status status = db->Put(rocksdb::WriteOptions(), "files.maxID", "0");

        if (!status.ok()) throw DBException();

        fileId = 0;
    }else {
        //If here is because id counter was initialized
        int maxID = stoi(value);
        rocksdb::Status status = db->Put(rocksdb::WriteOptions(), "files.maxID", std::to_string(maxID+1));

        if (!status.ok()) throw DBException();

        fileId = maxID + 1;
    }

    //At this point, fileId has a valid number and db id counter is initialized
    //The new id is assigned to the file.
    this->metadata->id = fileId;


}

void File::load(rocksdb::DB* db){

    int id = this->metadata->id;
    if(id < 0) throw FileNotFoundException(); //File id not set

    std::string value;
    rocksdb::Status status = db->Get(rocksdb::ReadOptions(),"files."+std::to_string(id),&value);
    if(status.IsNotFound()) throw FileNotFoundException();


    //If here is because file exists in db
    Json::Reader reader;
    Json::Value root;

    if(!reader.parse(value,root,false)) throw FileException();

    //Load metadata into file
    this->metadata->owner = root["owner"].asString();
    this->metadata->lastModified = root["lastModified"].asString();
    this->metadata->extension = root["extension"].asString();
    this->metadata->lastUser = root["lastUser"].asString();
    this->metadata->name = root["name"].asString();
    Json::Value tags = root["tags"];
    for(Json::Value::iterator it= tags.begin(); it != tags.end();it++ ){
        this->metadata->tags->push_back((*it).asString());
    }

}


void File::save(rocksdb::DB* db){




    int fileId = this->metadata->id;

    if(fileId < 0){ //Means that the file is new, doesn't exist in the db
        this->genId(db); //WARNING:Modifies id value
    }

    //At this point the file will have a valid id, needs to ve retrieved from atribute again
    fileId = this->metadata->id;

    //Saves file into id hash
    std::string fileKey = this->getKey();
    rocksdb::Status status = db->Put(rocksdb::WriteOptions(),"files.keys."+std::string(fileKey),std::to_string(fileId));

    if(!status.ok()) throw DBException();

    //Saves file
    Json::Value root = this->getJson();
    Json::StyledWriter writer;

    std::string json = writer.write(root);

    status = db->Put(rocksdb::WriteOptions(),"files."+std::to_string(fileId),json);

    if(!status.ok()) throw DBException();



}



void File::erase(rocksdb::DB* db){

    int fileId = this->metadata->id;

    if(fileId < 0) throw FileNotFoundException();



    //Removes file from id hash
    std::string fileKey = this->getKey();
    rocksdb::Status status = db->Delete(rocksdb::WriteOptions(),"files.keys."+std::string(fileKey));

    if(!status.ok()) throw DBException();



    //Gets file data
    std::string value;
    status = db->Get(rocksdb::ReadOptions(),"files."+std::to_string(fileId),&value);
    if(status.IsNotFound()) throw FileNotFoundException();


    //Removes file from db
    status = db->Delete(rocksdb::WriteOptions(),"files."+std::to_string(fileId));
    if(!status.ok()) throw DBException();



    //Saves file into trash
    status = db->Put(rocksdb::WriteOptions(),"files.trash."+std::to_string(fileId),value);

    if(!status.ok()) throw DBException();


}
