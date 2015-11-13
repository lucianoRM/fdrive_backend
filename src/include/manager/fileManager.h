//
// Created by luciano on 03/10/15.
//

#ifndef TALLER_FILEMANAGER_H
#define TALLER_FILEMANAGER_H

#include "mongoose.h"
#include "file.h"
#include "Manager.h"
#include <string>
#include <iostream>

// Class to manage the requests related to files.

class FileManager: public Manager {

    private:
        void shareFileToUser(int id, std::string email);

    public:
        FileManager();
        ~FileManager();

        File* openFile(int id);

        std::string saveFile(std::string email, std::string name, std::string extension, std::string path, std::vector<std::string> tags, int size); // Saves new file.
        std::string saveNewVersionOfFile(std::string email, int id, int oldVersion, bool overwrite, std::string name, std::string extension, std::vector<std::string> tags, int size); // It only replaces the old one. It should save different versions.
        std::string loadFile(int id); //Loads file

        void checkIfUserHasFilePermits(int id, std::string email);
        void checkIfUserIsOwner(int id, std::string email);

        std::string eraseFileFromUser(int id, std::string email, std::string path);

		std::string shareFileToUsers(int id, std::vector<std::string> users);
        
};

#endif //TALLER_FILEMANAGER_H
