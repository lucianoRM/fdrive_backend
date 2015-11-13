//
// Created by martin on 13/11/15.
//

#ifndef FDRIVE_FOLDERMANAGER_H
#define FDRIVE_FOLDERMANAGER_H

class FolderManager: public Manager {

private:

public:
    FolderManager();
    ~FolderManager();

    File* openFile(int id);

    std::string addFolder(std::string email, std::string path, std::string nameFolder); // Saves new file.

};


#endif //FDRIVE_FOLDERMANAGER_H
