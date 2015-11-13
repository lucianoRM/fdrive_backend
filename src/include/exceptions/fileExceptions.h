#ifndef TALLER_FILEEXCEPTIONS_H
#define TALLER_FILEEXCEPTIONS_H

#include <iostream>
#include <exception>

// Classes to represent the Exceptions related to Files.

class FileException: public std::exception {

    virtual const char* what() const throw() {
        return "Error with file";
    }

};

class FileExtensionNotValidException: public FileException {

    virtual const char* what() const throw() {
        return "File extension not valid";
    }

};

class FilenameNotValidException: public FileException {

    virtual const char* what() const throw() {
        return "File name not valid";
    }

};

class FileNotFoundException: public FileException {

    virtual const char* what() const throw() {
        return "File not found";
    }

};

class UserAlreadyHasFileSharedException: public FileException {

    virtual const char* what() const throw() {
        return "File already shared to user";
    }

};

class LastFileVersionNotDownload : public FileException {

    virtual const char* what() const throw() {
        return "Trying to update a file without viewing its last version.";
    }

};

class InexistentVersion : public FileException {

    virtual const char* what() const throw() {
        return "The version specified does not exists.";
    }

};

#endif //TALLER_FILEEXCEPTIONS_H
