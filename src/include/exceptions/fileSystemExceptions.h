#ifndef TALLER_FILESYSTEMEXCEPTIONS_H
#define TALLER_FILESYSTEMEXCEPTIONS_H

#include <iostream>
#include <exception>

// Classes to represent the Exceptions related to Files.

class FileSystemException: public std::exception {

    virtual const char* what() const throw() {
        return "Error in the file system";
    }

};

#endif //TALLER_FILESYSTEMEXCEPTIONS_H