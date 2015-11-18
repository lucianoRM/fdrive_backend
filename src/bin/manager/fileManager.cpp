//
// Created by luciano on 03/10/15.
//

#include <folder/folder.h>
#include "fileManager.h"
#include "userManager.h"
#include "folderManager.h"
#include "folderExceptions.h"
#include "fileExceptions.h"
#include <stdio.h>

FileManager::FileManager(rocksdb::DB* database) : Manager(database) { }
FileManager::~FileManager() { }

std::string FileManager::saveFile(std::string email, std::string name, std::string extension, std::string path, std::vector<std::string> tags, int size) {
    if (path.compare("trash") == 0 || path.compare("shared") == 0) throw InvalidFilePathException();
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

    UserManager u_manager = UserManager(this->database);
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
        ////delete db;
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
        ////delete db;
        ///std::cout << "ERROR2 pero cerré la base de datos en SaveFile." << std::endl;
        throw;
    }
    delete file;
    delete folder;
    ////delete db;

    u_manager.addFile(email, size);

    return "{ \"result\" : true , \"fileID\" : " + std::to_string(fileID) + " , \"version\" : 0 }";
}

std::string FileManager::changeFileData(int id, std::string name, std::string tag) {
    File* file = this->openFile(id);
    File* oldFile = this->openFile(id);
    std::string oldName = file->getName();
    std::string extension = file->getExtension();
    if (!name.empty()) file->setName(name);
    if (!tag.empty()) file->setTag(tag);

    rocksdb::DB* db = NULL;
    try {
        db = this->openDatabase("En ChangeFileData: ",'w');
        FolderManager f_manager(db);
        if (!name.empty()) {
            f_manager.renameFile(oldName + extension, name + extension, file->getOwner(), file->getOwnerPath());
            for (std::string user : file->getUsers()) {
                f_manager.renameFile(oldName, name, user, "shared");
            }
        }

        file->save(db);
    } catch(std::exception& e) {
        delete file;
        //if (db != NULL) ////delete db;
        throw;
    }

    // Change search info for owner and all shared users
    if (oldFile->getMetadata()->name.compare(name) != 0 || !tag.empty()) {
        file->changeSearchInformation(db,oldFile);
    }

    delete file;
    return "{ \"result\" : true }";
}

std::string FileManager::saveNewVersionOfFile(std::string email, int id, int oldVersion, bool overwrite, std::string name, std::string extension, std::vector<std::string> tags, int size) {
    File* file = this->openFile(id);

    int latestVersion = file->getLatestVersion();
    if (latestVersion < oldVersion) {
        delete file;
        throw InexistentVersion();
    }
    if (!overwrite) {
        if (latestVersion > oldVersion) {
            delete file;
            throw LastFileVersionNotDownload();
        }
    }

    File* oldFile;
    try {
        oldFile = this->openFile(id);
    } catch(std::exception& e) {
        delete file;
        throw;
    }
    int oldSize = oldFile->getSize();
    std::string owner = oldFile->getOwner();
    std::string ownerPath = oldFile->getMetadata()->ownerPath;
    std::string path = "shared";
    // The user is the owner
    if (owner.compare(email) == 0) {
        path = oldFile->getMetadata()->ownerPath;
        //std::cout << "\nNEW PATH: " << path;
    }

    file->startNewVersion();
    file->setName(name);
    file->setExtension(extension);
    file->setLastUser(email);
    file->setSize(size);
    file->setOwnerPath(ownerPath);
    for (std::string tag : tags) {
        file->setTag(tag);
    }
    int newVersion = file->getLatestVersion();

    UserManager u_manager = UserManager(this->database);
    try {
        u_manager.checkFileSizeChange(owner, oldSize, size);
    } catch (std::exception& e) {
        delete file;
        delete oldFile;
        ///std::cout << "ERROR1 pero cerré la base de datos en SaveFile." << std::endl;
        throw;
    }

    Folder* folder = NULL;
    rocksdb::DB* db = this->openDatabase("En SaveNewVersionOfFile: ",'w');
    try {
        folder = Folder::load(db,email,path);
        folder->removeFile(id);
        folder->addFile(id,name+extension);
        folder->save(db);

        file->save(db);
        oldFile->removeSearchInformation(db,email);
        file->saveSearches(email,path,db);
    } catch(std::exception& e) {
        delete oldFile;
        delete file;
        delete folder;
        ////delete db;
        throw; // Needs to be this way. If you throw e, a new instance is created and the exception class is missed,
    }
    ////delete db;
    delete file;
    delete oldFile;
    delete folder;

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
        if (db != NULL) ////delete db;
        throw;
    }
    ////delete db;
    return file;
}

std::string FileManager::loadFile(int id) {
    File* file = this->openFile(id);

    Json::StyledWriter writer;
    std::string json = "{ \"result\" : true , \"file\" : " + writer.write(file->getJson()) + " }";
    delete file;
    return json;
}

std::string FileManager::loadFile(int id, int version) {
    File* file = this->openFile(id);
    try {
        Json::StyledWriter writer;
        std::string json = "{ \"result\" : true , \"file\" : " + writer.write(file->getJson(version)) + " }";
        delete file;
        return json;
    } catch(std::exception& e) {
        delete file;
        throw;
    }
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
	File* file = this->openFile(id); // Tira excepción si no existe.
	delete file;
    for (std::string user : users) {
        this->checkFileSharedToUser(id, user);
    }
    for (std::string user : users) {
		this->shareFileToUser(id, user);
	}
	return "{ \"result\" : true }";
}

void FileManager::checkFileSharedToUser(int id, std::string email) {
    UserManager u_manager = UserManager(this->database);
    if (!u_manager.checkExistentUser(email)) {
        throw NonExistentUserException();
    }
    File* file = this->openFile(id);
    try {
        file->addSharedUser(email);
    } catch (std::exception& e) {
        delete file;
        throw;
    }
    delete file;
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
        if (db != NULL) ////delete db;
        if (folder != NULL) delete folder;
        delete file;
        throw;
    }
    delete file;
    ////delete db;
    delete folder;
}

std::string FileManager::shareFolder(std::string email, std::string path, std::vector<std::string> users) {
    FolderManager f_manager = FolderManager(this->database);
    std::vector<int> files = f_manager.getFilesFromFolder(email, path);
    for (std::string user : users) {
        for (int id : files) {
            try {
                this->shareFileToUser(id, user);
            } catch (FileAlreadyInFolderException &ex) {
                continue;
            }
        }
    }
    return "{ \"result\" : true }";
}


std::string FileManager::eraseFileFromUser(int id, std::string email, std::string path) {
    File* file = this->openFile(id);
    rocksdb::DB *db = NULL;
    try {
        db = this->openDatabase("En eraseFileFromUser: ",'w');
        file->eraseFromUser(db, email, path);
        file->save(db);
    } catch (std::exception& e) {
        if (db != NULL) ////delete db;
        delete file;
        throw;
    }

    // If we are in this point the logical remove (metadata) was correct
    std::string result = "true";
    /* Physical remove or move isn't required because files are saved by id.
     * bool isOwner = (email.compare(file->getOwner()) == 0);
    if (isOwner) {
        file->saveSearches(email,"trash",db);
        //int success = rename(("files/" + email + "/" + path + "/" + std::to_string(id)).c_str(),
        //                     ("files/" + email + "/trash/" + std::to_string(id)).c_str());
        //if (success != 0) {
        //    result = "false";
        }
    }*/

    delete file;
    ////delete db;
    return "{ \"result\" : " + result + " }";
}

std::string FileManager::getSearches(std::string email, std::string typeOfSearch, std::string element) {
    rocksdb::DB* db = openDatabase("En get Searches",'r');
    ///std::cout << "Abrí la base de datos en LoadUserFiles." << std::endl;
    SearchInformation* search = NULL;

    try {
        search = SearchInformation::load(db, typeOfSearch, email, element);
    } catch (std::exception& e) {
        if (search != NULL) delete search;
        ////delete db;
        throw;
    }

    std::string content = search->getContent();
    delete search;
    ////delete db;
    ///std::cout << "Cerré la base de datos en LoadUserFiles." << std::endl;
    return "{ \"result\" : true , \"content\" : " + content + " }";
}

std::string FileManager::recoverFile(std::string email, int id) {
    File* file = this->openFile(id);
    // We don't have to check if the user is the owner, because only the owner can recover something from the trash.
    std::string path = file->getOriginalOwnerPath();
    rocksdb::DB *db = NULL;
    try {
        db = this->openDatabase("En recoverFile: ",'w');
        file->recoverFromUser(db, email, path);
        file->save(db);
    } catch (std::exception& e) {
        if (db != NULL) ////delete db;
        delete file;
        throw;
    }

    // If we are in this point the logical remove (metadata) was correct
    /* Physical movement isn't needed.
    int success = rename(("files/" + email + "/trash/" + std::to_string(id)).c_str(),
                         ("files/" + email + "/" + path + "/" + std::to_string(id)).c_str());
    if (success != 0) {
        result = "false";
    }
     */
    delete file;
    ////delete db;
    return "{ \"result\" : true , \"path\" : " + path + " }";
}

std::string FileManager::deleteFileSharedPermits(int id, std::vector<std::string> users) {
    File* file = this->openFile(id);
    if (users.empty()) {
        std::list<std::string> l = file->getUsers();
        users = std::vector<std::string>( std::make_move_iterator(std::begin(l)),
                                          std::make_move_iterator(std::end(l)) );
    }
    rocksdb::DB *db = NULL;
    try {
        db = this->openDatabase("En deleteFileSharedPermits: ",'w');
    } catch (std::exception& e) {
        delete file;
        throw;
    }
    for (std::string user: users) {
        try {
            file->eraseFromUser(db, user, "shared");
        } catch (HasNoPermits& ex) {
            continue;
        } catch (std::exception& e) {
            delete file;
            ////delete db;
            throw;
        }
    }
    try {
        file->save(db);
    } catch (std::exception& e) {
        delete file;
        ////delete db;
        throw;
    }

    delete file;
    ////delete db;
    return "{ \"result\" : true }";
}
