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
#include "fileExceptions.h"
#include "folderExceptions.h"

// Class to manage the requests related to files.

class FileManager: public Manager {

    private:
        void shareFileToUser(int id, std::string email);
        void checkFileSharedToUser(int id, std::string email);

    public:
        FileManager();
        ~FileManager();

        File* openFile(int id);

        std::string saveFile(std::string email, std::string name, std::string extension, std::string path, std::vector<std::string> tags, int size); // Saves new file.
        std::string saveNewVersionOfFile(std::string email, int id, int oldVersion, bool overwrite, std::string name, std::string extension, std::vector<std::string> tags, int size); // It only replaces the old one. It should save different versions.
        std::string loadFile(int id); //Loads file
        std::string loadFile(int id, int version); //Loads the specified version of file
        std::string changeFileData(int id, std::string name, std::string tag);

        void checkIfUserHasFilePermits(int id, std::string email);
        void checkIfUserIsOwner(int id, std::string email);

        std::string eraseFileFromUser(int id, std::string email, std::string path);

		std::string shareFileToUsers(int id, std::vector<std::string> users);
        std::string shareFolder(std::string email, std::string path, std::vector<std::string> users);
        // Elimina el archivo de los users que se pasan, si está vacío, se elimina de todos los que tenían al archivo compartido.
        // Si alguno de la lista, no tenía el archivo compartido, lo saltea, lo ignora.
        std::string deleteFileSharedPermits(int id, std::vector<std::string> users);

        std::string getSearches(std::string email, std::string typeOfSearch, std::string element);

        std::string recoverFile(std::string email,int id);
        
};

#endif //TALLER_FILEMANAGER_H
