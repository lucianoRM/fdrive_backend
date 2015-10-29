//
// Created by luciano on 08/10/15.
//

#ifndef TALLER_FILEEXCEPTIONS_H
#define TALLER_FILEEXCEPTIONS_H



#include <iostream>
#include <exception>



//Default message
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

class FilenameTakenException: public FileException {

    virtual const char* what() const throw() {
        return "File name already taken";
    }

};

class FileNotFoundException: public FileException {

    virtual const char* what() const throw() {
        return "File not found";
    }

};




#endif //TALLER_FILEEXCEPTIONS_H
