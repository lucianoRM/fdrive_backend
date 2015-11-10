#include "file.h"

File::File() {
    this->metadata = new struct metadata;
    this->metadata->id = -1;
    time_t currTime;
    time(&currTime);
    char* time = ctime(&currTime);
    time[strlen(time) - 1] = '\0'; // Removes \n at the end.
    this->metadata->lastModified = std::string(time);
    this->metadata->tags = new std::list<std::string>();
    this->users = new std::list<std::string>();
}

File::~File() {
    delete this->users;
    delete this->metadata->tags;
    delete this->metadata;
}

void File::setName(std::string newName) {
    if(newName == "~") throw FilenameNotValidException();
    this->metadata->name = newName;
}

void File::setExtension(std::string newExt) {
    if(newExt == ".") throw FileExtensionNotValidException();
    this->metadata->extension = newExt;
}

void File::setOwner(std::string newOwner) {
    this->metadata->owner = newOwner;
}

void File::setOwnerPath(std::string newPath) {
    this->metadata->ownerPath = newPath;
}

void File::setLastModDate() {
    time_t currTime;
    time(&currTime);
    char* time = ctime(&currTime);
    time[strlen(time) - 1] = '\0'; // Removes \n at the end.
    this->metadata->lastModified = std::string(time);
}

void File::setLastUser(std::string newLastUser) {
    this->metadata->lastUser = newLastUser;
}

void File::setTag(std::string newTag) {
    std::list<std::string>* ftags = this->metadata->tags;

    // Checks if the tag already exists.
    bool exists = false;
    std::for_each(ftags->begin(), ftags->end(), [&newTag,&exists](std::string &n){if(!n.compare(newTag)) {exists = true; return;} });

    if(!exists) this->metadata->tags->push_back(newTag);
}

void File::setId(int id) {
    this->metadata->id = id;
}

struct metadata* File::getMetadata() {
    return this->metadata;
}

std::string File::getKey() {
    std::string key = this->metadata->name + this->metadata->extension + ":" + this->metadata->owner;
    return key;
}

int File::getId() {
    return this->metadata->id;
}

Json::Value File::getJson() {
    Json::Value root;
    root["name"] = this->metadata->name;
    root["extension"]  = this->metadata->extension;
    root["owner"] = this->metadata->owner;
    root["pathInOwner"] = this->metadata->ownerPath;
    root["id"] = this->metadata->id;
    root["lastModified"] = this->metadata->lastModified;
    root["lastUser"] = this->metadata->lastUser;

    Json::Value tags (Json::arrayValue);
    std::for_each(this->metadata->tags->begin(),this->metadata->tags->end(),[&tags](std::string &tag){tags.append(tag);});
    root["tags"] = tags;

    Json::Value users (Json::arrayValue);
    std::for_each(this->users->begin(),this->users->end(),[&users](std::string &user){users.append(user);});
    root["users"] = users;

    return root;
}

// If this function is called is because a file with no id(-1) is trying to be saved.
void File::genId(rocksdb::DB* db) {
    // Gets id counter.
    int fileId;
    std::string value;
    rocksdb::Status status = db->Get(rocksdb::ReadOptions(), "files.maxID", &value);

    // if not found has to initialize it.
    if (status.IsNotFound()) {
        rocksdb::Status status = db->Put(rocksdb::WriteOptions(), "files.maxID", "0");

        if (! status.ok()) throw DBException();
        fileId = 0;
    }else {
        // If here is because id counter was initialized.
        int maxID = stoi(value);
        rocksdb::Status status = db->Put(rocksdb::WriteOptions(), "files.maxID", std::to_string(maxID+1));

        if (!status.ok()) throw DBException();
        fileId = maxID + 1;
    }

    // At this point, fileId has a valid number and db id counter is initialized.
    // The new id is assigned to the file.
    this->metadata->id = fileId;
}

File* File::load(rocksdb::DB* db, int id) {
    File* file = new File();
    file->setId(id);
    try {
        file->load(db);
    } catch(std::exception& e) {
        delete file;
        throw; // Needs to be this way. If you throw e, a new instance is created and the exception class is missed,
    }
    return file;
}

void File::load(rocksdb::DB* db) {
    int id = this->metadata->id;
    if (id < 0) throw FileNotFoundException(); //File id not set.

    std::string value;
    rocksdb::Status status = db->Get(rocksdb::ReadOptions(),"files."+std::to_string(id),&value);
    if (status.IsNotFound()) throw FileNotFoundException();

    // If here is because file exists in db.
    Json::Reader reader;
    Json::Value root;

    if (! reader.parse(value,root,false)) throw FileException();

    // Load metadata into file.
    this->metadata->owner = root["owner"].asString();
    this->metadata->ownerPath = root["pathInOwner"].asString();
    this->metadata->lastModified = root["lastModified"].asString();
    this->metadata->extension = root["extension"].asString();
    this->metadata->lastUser = root["lastUser"].asString();
    this->metadata->name = root["name"].asString();

    Json::Value tags = root["tags"];
    for (Json::Value::iterator it = tags.begin(); it != tags.end(); it++) {
        this->metadata->tags->push_back((*it).asString());
    }

    Json::Value users = root["users"];
    for (Json::Value::iterator it = users.begin(); it != users.end(); it++) {
        this->users->push_back((*it).asString());
    }
}


void File::save(rocksdb::DB* db) {
    int fileId = this->metadata->id;

    if (fileId < 0) { // Means that the file is new, doesn't exist in the db.
        this->genId(db); // WARNING:Modifies id value.
    }

    // At this point the file will have a valid id, needs to ve retrieved from atribute again.
    fileId = this->metadata->id;

    // Saves file,
    Json::Value root = this->getJson();
    Json::StyledWriter writer;

    std::string json = writer.write(root);

    rocksdb::Status status = db->Put(rocksdb::WriteOptions(),"files." + std::to_string(fileId), json);

    if (!status.ok()) throw DBException();
}

void File::saveSearches(std::string user, std::string path, rocksdb::DB* db) {
    SearchInformation* owner = NULL;
    try {
        owner =  SearchInformation::load(db, "owner", user, this->metadata->owner);
        owner->addFile(this->metadata->id, path);
        owner->save(db);
    } catch (std::exception& e) {
        if (owner != NULL) delete owner;
        throw;
    }
    delete owner;

    SearchInformation* name = NULL;
    try {
        name = SearchInformation::load(db, "name", user, this->metadata->name);
        name->addFile(this->metadata->id, path);
        name->save(db);
    } catch (std::exception& e) {
        if (name != NULL) delete name;
        throw;
    }
    delete name;


    SearchInformation* extension = NULL;
    try {
        extension = SearchInformation::load(db, "extension", user, this->metadata->extension);
        extension->addFile(this->metadata->id, path);
        extension->save(db);
    } catch (std::exception& e) {
        if (extension != NULL) delete extension;
        throw;
    }
    delete extension;

    for (std::string tag : *(this->metadata->tags)) {
        SearchInformation* tagInfo = NULL;
        try {
            tagInfo = SearchInformation::load(db, "tag", user, tag);
            tagInfo->addFile(this->metadata->id, path);
            tagInfo->save(db);
        }  catch (std::exception& e) {
            if (tagInfo != NULL) delete tagInfo;
            throw;
        }
        delete tagInfo;
    }
}

void File::changeSearchInformation(File* oldFile) {
    //TODO ver diferencias de tags, name y extension para cambiarlos. (owner no se puede cambiar)
}

void File::checkIfUserHasPermits(std::string email) {
    if (this->metadata->owner.compare(email) == 0) return;
    for (std::string user : *users) {
        if (user.compare(email) == 0) return;
    }
    throw HasNoPermits();
}

void File::checkIfUserIsOwner(std::string email) {
	if (this->metadata->owner.compare(email) == 0) return;
	throw IsNotTheOwner();
}

void File::eraseFromUser(rocksdb::DB* db, std::string user, std::string path) {
    if (user.compare(this->metadata->owner)) {
        for (std::string sharedUser : *this->users) {
            this->deleteFromUser(db, sharedUser, "shared");
        }
        this->deleteFromUser(db, user, path);
        Folder* folder = NULL;
        try {
            folder = Folder::load(db, user, "trash");
            folder->addFile(this->metadata->id, this->metadata->name + "." + this->metadata->extension);
            folder->save(db);
        } catch (std::exception& e) {
            if (folder != NULL) delete folder;
            throw;
        }
        delete folder;
        this->users->clear();
    } else {
        this->deleteFromUser(db, user, "shared");
        this->users->remove(user);
    }
}

void File::deleteFromUser(rocksdb::DB* db, std::string user, std::string path) {
    SearchInformation* owner = NULL;
    try {
        owner =  SearchInformation::load(db, "owner", user, this->metadata->owner);
        owner->eraseFile(this->metadata->id); //, path);
        owner->save(db);
    } catch (std::exception& e) {
        if (owner != NULL) delete owner;
        throw;
    }
    delete owner;

    SearchInformation* name = NULL;
    try {
        name = SearchInformation::load(db, "name", user, this->metadata->name);
        name->eraseFile(this->metadata->id); //, path);
        name->save(db);
    } catch (std::exception& e) {
        if (name != NULL) delete name;
        throw;
    }
    delete name;


    SearchInformation* extension = NULL;
    try {
        extension = SearchInformation::load(db, "extension", user, this->metadata->extension);
        extension->eraseFile(this->metadata->id); //, path);
        extension->save(db);
    } catch (std::exception& e) {
        if (extension != NULL) delete extension;
        throw;
    }
    delete extension;

    for (std::string tag : *(this->metadata->tags)) {
        SearchInformation* tagInfo = NULL;
        try {
            tagInfo = SearchInformation::load(db, "tag", user, tag);
            tagInfo->eraseFile(this->metadata->id); //, path);
            tagInfo->save(db);
        } catch (std::exception& e) {
            if (tagInfo != NULL) delete tagInfo;
            throw;
        }
        delete tagInfo;
    }

    Folder* folder = NULL;
    try {
        folder = Folder::load(db, user, path);
        folder->removeFile(this->metadata->id);
        folder->save(db);
    } catch (std::exception& e) {
        if (folder != NULL) delete folder;
        throw;
    }
    delete folder;
}

void File::addSharedUser(std::string user) {
    if (std::find(this->users->begin(), this->users->end(), user) != this->users->end()) {
        throw UserAlreadyHasFileSharedException();
    } else {
        this->users->push_back(user);
    }
}

std::list<std::string> File::getUsers() {
	return *this->users;
}

void File::setSize(int size) {
	this->metadata->size = size;
}

int File::getSize() {
	return this->metadata->size;
}
