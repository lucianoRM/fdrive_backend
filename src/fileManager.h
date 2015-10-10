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

class FileManager: public Manager{

    public:
        FileManager();
        ~FileManager();

        std::string saveFile(std::string name, std::string extension, std::string owner, std::vector<std::string> tags); //Saves file
        std::string loadFile(int id); //Loads file
        std::string eraseFile(int id); //erases file

};


#endif //TALLER_FILEMANAGER_H
