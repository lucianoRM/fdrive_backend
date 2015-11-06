//
// Created by martin on 1/11/15.
//

#ifndef FDRIVE_FOLDEREXCEPTIONS_H
#define FDRIVE_FOLDEREXCEPTIONS_H

#include <iostream>
#include <exception>

// Classes to represent the Exceptions related to Files.

class FolderException: public std::exception {

    virtual const char* what() const throw() {
        return "Error with folder";
    }

};

class AlreadyExistentFolderException: public FolderException {

    virtual const char* what() const throw() {
        return "Folder name already existent in current path";
    }

};

#endif //FDRIVE_FOLDEREXCEPTIONS_H
