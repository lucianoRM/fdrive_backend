//
// Created by martin on 7/10/15.
//

#ifndef FDRIVE_NONEXISTENTUSEREXCEPTION_H
#define FDRIVE_NONEXISTENTUSEREXCEPTION_H

#include <iostream>
#include <exception>

class NonExistentUserException: public std::exception {

    virtual const char* what() const throw()
    {
        return "Usuario No existente";
    }

};

#endif //FDRIVE_NONEXISTENTUSEREXCEPTION_H
