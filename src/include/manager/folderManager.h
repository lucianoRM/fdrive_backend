//
// Created by martin on 13/11/15.
//

#ifndef FDRIVE_FOLDERMANAGER_H
#define FDRIVE_FOLDERMANAGER_H

#include "Manager.h"
#include <vector>

class FolderManager: public Manager {

private:

public:
    FolderManager();
    ~FolderManager();

    std::string addFolder(std::string email, std::string path, std::string nameFolder); // Saves new folder
    std::string renameFolder(std::string email, std::string path, std::string oldName, std::string newName); //Saves the new name of the folder
    std::vector<int> getFilesFromFolder(std::string email, std::string path);
    void renameFile(std::string oldName, std::string newName, std::string email, std::string path);

};


#endif //FDRIVE_FOLDERMANAGER_H
