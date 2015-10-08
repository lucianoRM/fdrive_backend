//
// Created by martin on 7/10/15.
//

#ifndef FDRIVE_USEREXCEPTION_H
#define FDRIVE_USEREXCEPTION_H

#include <iostream>
#include <exception>

class UserException : public std::exception {
};

class NonExistentUserException : public UserException {
    virtual const char* what() const throw()
    {
        return "Usuario No existente";
    }
};

class AlreadyExistentUserException : public UserException {
    virtual const char* what() const throw()
    {
        return "El Usuario ya existe, por favor elija otro nombre";
    }
};

#endif //FDRIVE_USEREXCEPTION_H
