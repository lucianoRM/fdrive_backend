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

        bool saveFile(struct mg_connection* conn); //Saves file
        bool loadFile(struct mg_connection* conn); //Loads file


};


#endif //TALLER_FILEMANAGER_H
