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

#endif //TALLER_FILEEXCEPTIONS_H