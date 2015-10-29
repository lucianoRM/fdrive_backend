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

class FileManager: public Manager {

    private:
        File* openFile(int id);

    public:
        FileManager();
        ~FileManager();

        std::string saveFile(std::string email, std::string name, std::string extension, std::vector<std::string> tags); // Saves new file
        std::string saveNewVersionOfFile(std::string email, int id, std::string name, std::string extension, std::vector<std::string> tags); // Por ahora sólo reemplza el viejo. No guarda las distintas versiones.
        std::string loadFile(int id); //Loads file

};


#endif //TALLER_FILEMANAGER_H
