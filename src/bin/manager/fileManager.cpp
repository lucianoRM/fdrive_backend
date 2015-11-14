//
// Created by luciano on 03/10/15.
//

#include <folder/folder.h>
#include "fileManager.h"
#include "userManager.h"
#include <stdio.h>

FileManager::FileManager() { }
FileManager::~FileManager() { }



std::string FileManager::saveFile(std::string email, std::string name, std::string extension, std::string path, std::vector<std::string> tags, int size) {
    File* file = new File();
    file->setName(name);
    file->setExtension(extension);
    file->setOwner(email);
    file->setOwnerPath(path);
    file->setLastUser(email);
    file->setSize(size);
    for (std::string tag : tags ) {
        file->setTag(tag);
    }

    UserManager u_manager;
    try {
        u_manager.checkFileAddition(email, size);
    } catch (std::exception& e) {
        delete file;
        throw;
    }

    rocksdb::DB* db = this->openDatabase("En SaveFile: ",'w');
    ///std::cout << "Abrí la base de datos en SaveFile." << std::endl;

    Folder* folder = NULL;
    // To check if there is a existing file with the same name File in current path.
    try {
        folder = Folder::load(db, email, path);
        file->genId(db);
    } catch (std::exception& e) {
        delete file;
        if (folder != NULL) delete folder;
        delete db;
        ///std::cout << "ERROR1 pero cerré la base de datos en SaveFile." << std::endl;
        throw;
    }

    int fileID = file->getId();

    try {
        folder->addFile(fileID, name+extension);
        folder->save(db);
        file->save(db);
        file->saveSearches(email, path, db);
    } catch (std::exception& e) {
        delete file;
        delete folder;
        delete db;
        ///std::cout << "ERROR2 pero cerré la base de datos en SaveFile." << std::endl;
        throw;
    }
    delete file;
    delete folder;
    delete db;

    u_manager.addFile(email, size);

    return "{ \"result\" : true , \"fileID\" : " + std::to_string(fileID) + " }";
}

std::string FileManager::saveNewVersionOfFile(std::string email, int id, std::string name, std::string extension, std::vector<std::string> tags, int size) {
    File* file = this->openFile(id);
    File* oldFile;
    try {
        oldFile = this->openFile(id);
    } catch(std::exception& e) {
        delete file;
        throw;
    }
    int oldSize = oldFile->getSize();
    std::string owner = oldFile->getOwner();

    file->startNewVersion();
    file->setName(name);
    file->setExtension(extension);
    file->setLastUser(email);
    file->setSize(size);
    for (std::string tag : tags) {
        file->setTag(tag);
    }
    int newVersion = file->getLatestVersion();

    UserManager u_manager;
    try {
        u_manager.checkFileSizeChange(owner, oldSize, size);
    } catch (std::exception& e) {
        delete file;
        delete oldFile;
        ///std::cout << "ERROR1 pero cerré la base de datos en SaveFile." << std::endl;
        throw;
    }

    rocksdb::DB* db = this->openDatabase("En SaveNewVersionOfFile: ",'w');
    try {
        file->changeSearchInformation(oldFile);
        file->save(db);
    } catch(std::exception& e) {
        delete oldFile;
        delete file;
        delete db;
        throw; // Needs to be this way. If you throw e, a new instance is created and the exception class is missed,
    }
    delete db;
    delete file;
    delete oldFile;

    u_manager.changeFileSize(owner, oldSize, size);

    return "{ \"result\" : true , \"version\" : " + std::to_string(newVersion) + " }";
}

File* FileManager::openFile(int id) {
    File* file = new File();

    file->setId(id);

    rocksdb::DB* db = NULL;

    try {
        db = this->openDatabase("En OpenFile: ",'r');
        file->load(db);
    } catch(std::exception& e) {
        delete file;
        if (db != NULL) delete db;
        throw;
    }
    delete db;
    return file;
}

std::string FileManager::loadFile(int id) {
    File* file = this->openFile(id);

    Json::Value value;
    Json::StyledWriter writer;
    std::string json = writer.write(file->getJson());
    delete file;
    return json;
}

void FileManager::checkIfUserHasFilePermits(int id, std::string email) {
    File* file = this->openFile(id);
    try {
        file->checkIfUserHasPermits(email);
    } catch(std::exception& e) {
        delete file;
        throw;
    }
    delete file;
}

void FileManager::checkIfUserIsOwner(int id, std::string email) {
	File* file = this->openFile(id);
    try {
        file->checkIfUserIsOwner(email);
    } catch(std::exception& e) {
        delete file;
        throw;
    }
    delete file;
}

std::string FileManager::shareFileToUsers(int id, std::vector<std::string> users) {
	File* file = this->openFile(id);
	int size = file->getSize();
	delete file;
	UserManager u_manager;
	/* A shared file does not occupy quota in the others...
    for (std::string user : users) {
		u_manager.checkFileAddition(user, size);
	}
	*/
	for (std::string user : users) {
		this->shareFileToUser(id, user);
	}
    /* Ídem...
	for (std::string user : users) {
		u_manager.addFile(user, size);
	}
     */
	return "{ \"result\" : true }";
}

void FileManager::shareFileToUser(int id, std::string email) {
	File* file = this->openFile(id);
    rocksdb::DB *db = NULL;
    Folder* folder = NULL;
    try {
        db = this->openDatabase("En shareFileToUser: ",'w');
        file->addSharedUser(email);
        file->save(db);
        folder = Folder::load(db, email, "shared");
        folder->addFile(id, file->getMetadata()->name + file->getMetadata()->extension);
        folder->save(db);
        file->saveSearches(email, "shared", db);
    } catch (std::exception& e) {
        if (db != NULL) delete db;
        if (folder != NULL) delete folder;
        delete file;
        throw;
    }
}


std::string FileManager::eraseFileFromUser(int id, std::string email, std::string path) {
    File* file = this->openFile(id);
    rocksdb::DB *db = NULL;
    try {
        db = this->openDatabase("En eraseFileFromUser: ",'w');
        file->eraseFromUser(db, email, path);
        file->save(db);
    } catch (std::exception& e) {
        if (db != NULL) delete db;
        delete file;
        throw;
    }

    // If we are in this point the logical remove (metadata) was correct
    std::string result = "false";

    int success = rename( ("files/"+email+"/path/"+std::to_string(id)).c_str(),("files/"+email+"/trash/"+std::to_string(id)).c_str());
    if (success == 0) {
        result = "true";
    }

    delete file;
    delete db;
    return "{ \"result\" : " + result + " }";
}
