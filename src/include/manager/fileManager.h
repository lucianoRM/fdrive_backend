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
        File* openFile(int id);

    public:
        FileManager();
        ~FileManager();

        std::string saveFile(std::string email, std::string name, std::string extension, std::string path, std::vector<std::string> tags, int size); // Saves new file.
        std::string saveNewVersionOfFile(std::string email, int id, std::string name, std::string extension, std::vector<std::string> tags); // It only replaces the old one. It should save different versions.
        std::string loadFile(int id); //Loads file

        void checkIfUserHasFilePermits(int id, std::string email);

        std::string eraseFileFromUser(int id, std::string email, std::string path);

        std::string shareFileToUser(int id, std::string email);

};

#endif //TALLER_FILEMANAGER_H
