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
        return "The User does not exist";
    }
};

class AlreadyExistentUserException : public UserException {
    virtual const char* what() const throw()
    {
        return "Already existent user. Please choose another name";
    }
};

class WrongPasswordException : public UserException {
    virtual const char* what() const throw()
    {
        return "Invalid Password";
    }
};

#endif //FDRIVE_USEREXCEPTION_H
