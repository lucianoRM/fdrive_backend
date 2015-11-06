//
// Created by martin on 1/11/15.
//

#ifndef FDRIVE_FOLDEREXCEPTIONS_H
#define FDRIVE_FOLDEREXCEPTIONS_H

#include <iostream>
#include <exception>

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


class FolderNotFound: public std::exception {

    virtual const char* what() const throw() {
        return "Folder nor found";
    }

};

class FilenameTakenException: public FolderException {

    virtual const char* what() const throw() {
        return "File name already taken";
    }

};

class FileAlreadyInFolderException: public FolderException {

    virtual const char* what() const throw() {
        return "File already in folder";
    }

};

class FileNotInFolderException: public FolderException {

    virtual const char* what() const throw() {
        return "File not in folder";

    }

};


#endif //FDRIVE_FOLDEREXCEPTIONS_H
