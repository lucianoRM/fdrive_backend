#include "file.h"

File::File() {
    this->id = -1;
    this->lastVersion = 0;
    Version* version = new Version();
    this->versions = new std::unordered_map<int, Version*>();
    (*this->versions)[0] = version;
    this->users = new std::list<std::string>();
}

File::~File() {
    delete this->users;
    for (auto version : (*this->versions)) {
        delete version.second;
    }
    delete this->versions;
}

void File::setName(std::string newName) {
    (*this->versions)[this->lastVersion]->setName(newName);
}

void File::setExtension(std::string newExt) {
    (*this->versions)[this->lastVersion]->setExtension(newExt);
}

void File::setOwner(std::string newOwner) {
    this->owner = newOwner;
}

void File::setOwnerPath(std::string newPath) {
    (*this->versions)[this->lastVersion]->setOwnerPath(newPath);
}

void File::setLastModDate() {
    (*this->versions)[this->lastVersion]->setLastModDate();
}

void File::setLastUser(std::string newLastUser) {
    (*this->versions)[this->lastVersion]->setLastUser(newLastUser);
}

void File::setTag(std::string newTag) {
    (*this->versions)[this->lastVersion]->setTag(newTag);
}

void File::setId(int id) {
    this->id = id;
}

struct metadata* File::getMetadata() {
    return (*this->versions)[this->lastVersion]->getMetadata();
}

int File::getId() {
    return this->id;
}

std::string File::getOwner() {
    return this->owner;
}

int File::getLatestVersion() {
    return this->lastVersion;
}

Json::Value File::getJson() {
    Json::Value root = this->getJson(this->lastVersion);
    root["id"] = this->id;
    root = this->addFileData(root);
    return root;
}

Json::Value File::addFileData(Json::Value json) {
    json["lastVersion"] = this->lastVersion;
    json["owner"] = this->owner;

    Json::Value users (Json::arrayValue);
    std::for_each(this->users->begin(),this->users->end(),[&users](std::string &user){users.append(user);});
    json["users"] = users;

    return json;
}

Json::Value File::getJson(int version) {
    return (*this->versions)[version]->getJson();
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
    this->id = fileId;
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
    int id = this->id;
    if (id < 0) throw FileNotFoundException(); //File id not set.

    std::string value;
    rocksdb::Status status = db->Get(rocksdb::ReadOptions(),"files."+std::to_string(id),&value);
    if (status.IsNotFound()) throw FileNotFoundException();

    // If here is because file exists in db.
    Json::Reader reader;
    Json::Value root;

    if (! reader.parse(value,root,false)) throw FileException();

    // Load metadata into file.
    this->owner = root["owner"].asString();
    this->lastVersion = root["lastVersion"].asInt();

    Json::Value users = root["users"];
    for (Json::Value::iterator it = users.begin(); it != users.end(); it++) {
        this->users->push_back((*it).asString());
    }

    Json::Value versions = root["versions"];
    for (int i = 0; i <= this->lastVersion; i++) {
        (*this->versions)[i] = Version::load(versions[std::to_string(i)]);
    }

    // Y cambia el lastModified al actual.
    (*this->versions)[this->lastVersion]->setLastModDate();
}


void File::save(rocksdb::DB* db) {
    if (this->id < 0) { // Means that the file is new, doesn't exist in the db.
        this->genId(db); // WARNING: Modifies id value.
    }

    Json::Value root;
    root = this->addFileData(root);

    Json::Value versions;
    for (auto version : (*this->versions)) {
        versions[std::to_string(version.first)] = version.second->getJson();
    }
    root["versions"] = versions;

    // Saves file,
    Json::StyledWriter writer;
    rocksdb::Status status = db->Put(rocksdb::WriteOptions(),"files." + std::to_string(this->id), writer.write(root));

    if (!status.ok()) throw DBException();
}

void File::saveSearches(std::string user, std::string path, rocksdb::DB* db) {
    SearchInformation* owner = NULL;
    try {
        owner =  SearchInformation::load(db, "owner", user, this->owner);
        owner->addFile(this->id, path);
        owner->save(db);
    } catch (std::exception& e) {
        if (owner != NULL) delete owner;
        throw;
    }
    delete owner;

    struct metadata* metadata = (*this->versions)[this->lastVersion]->getMetadata();
    SearchInformation* name = NULL;
    try {
        name = SearchInformation::load(db, "name", user, metadata->name);
        name->addFile(this->id, path);
        name->save(db);
    } catch (std::exception& e) {
        if (name != NULL) delete name;
        throw;
    }
    delete name;


    SearchInformation* extension = NULL;
    try {
        extension = SearchInformation::load(db, "extension", user, metadata->extension);
        extension->addFile(this->id, path);
        extension->save(db);
    } catch (std::exception& e) {
        if (extension != NULL) delete extension;
        throw;
    }
    delete extension;

    for (std::string tag : *(metadata->tags)) {
        SearchInformation* tagInfo = NULL;
        try {
            tagInfo = SearchInformation::load(db, "tag", user, tag);
            tagInfo->addFile(this->id, path);
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
    if (this->owner.compare(email) == 0) return;
    for (std::string user : *users) {
        if (user.compare(email) == 0) return;
    }
    throw HasNoPermits();
}

void File::checkIfUserIsOwner(std::string email) {
	if (this->owner.compare(email) == 0) return;
	throw IsNotTheOwner();
}

void File::eraseFromUser(rocksdb::DB* db, std::string user, std::string path) {
    struct metadata* metadata = (*this->versions)[this->lastVersion]->getMetadata();
    if (user.compare(this->owner)) {
        //for (std::string sharedUser : *this->users) {
          //  this->deleteFromUser(db, sharedUser, "shared");
        //}
        //this->deleteFromUser(db, user, path);
        Folder* folder = NULL;
        try {
            folder = Folder::load(db, user, "trash");
            folder->addFile(this->id, metadata->name + "." + metadata->extension);
            folder->save(db);
            delete folder;
            folder = Folder::load(db, user, path);
            folder->removeFile(this->getId());
            folder->save(db);
        } catch (std::exception& e) {
            if (folder != NULL) delete folder;
            throw;
        }
        delete folder;
        this->users->clear();
    } else {
        //this->deleteFromUser(db, user, "shared");
        this->users->remove(user);
    }
}
/*
void File::deleteFromUser(rocksdb::DB* db, std::string user, std::string path) {

    SearchInformation* owner = NULL;
    try {
        owner =  SearchInformation::load(db, "owner", user, this->owner);
        owner->eraseFile(this->getId()); //, path);
        owner->save(db);
    } catch (std::exception& e) {
        if (owner != NULL) delete owner;
        throw;
    }
    delete owner;

    struct metadata* metadata = (*this->versions)[this->lastVersion]->getMetadata();
    SearchInformation* name = NULL;
    try {
        name = SearchInformation::load(db, "name", user, metadata->name);
        name->eraseFile(this->id); //, path);
        name->save(db);
    } catch (std::exception& e) {
        if (name != NULL) delete name;
        throw;
    }
    delete name;


    SearchInformation* extension = NULL;
    try {
        extension = SearchInformation::load(db, "extension", user, metadata->extension);
        extension->eraseFile(this->id); //, path);
        extension->save(db);
    } catch (std::exception& e) {
        if (extension != NULL) delete extension;
        throw;
    }
    delete extension;

    for (std::string tag : *(metadata->tags)) {
        SearchInformation* tagInfo = NULL;
        try {
            tagInfo = SearchInformation::load(db, "tag", user, tag);
            tagInfo->eraseFile(this->id); //, path);
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
        folder->removeFile(this->id);
        folder->save(db);
    } catch (std::exception& e) {
        if (folder != NULL) delete folder;
        throw;
    }
    delete folder;
}
*/

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
	(*this->versions)[this->lastVersion]->setSize(size);
}

int File::getSize() {
	return (*this->versions)[this->lastVersion]->getSize();
}

void File::startNewVersion() {
    this->lastVersion++;
    (*this->versions)[this->lastVersion] = new Version();
}