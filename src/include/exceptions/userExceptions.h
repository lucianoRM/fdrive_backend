//
// Created by martin on 7/10/15.
//

#ifndef FDRIVE_USEREXCEPTION_H
#define FDRIVE_USEREXCEPTION_H

#include <iostream>
#include <exception>

// Classes to represent the Exceptions related to Users.

class UserException : public std::exception {

    virtual const char* what() const throw() {
        return "Error with user";
    }

};

class NonExistentUserException : public UserException {

    virtual const char* what() const throw() {
        return "The User does not exist";
    }

};

class AlreadyExistentUserException : public UserException {

    virtual const char* what() const throw() {
        return "Already existent user. Please choose another name";
    }

};

class WrongPasswordException : public UserException {

    virtual const char* what() const throw() {
        return "Invalid Password";
    }

};

class NotLoggedInException : public UserException {

    virtual const char* what() const throw() {
        return "Not logged in";
    }

};

class HasNoPermits : public UserException {

    virtual const char* what() const throw() {
        return "The user has no permits for specified file.";
    }

};

class NotEnoughQuota : public UserException {
	virtual const char* what() const throw() {
        return "The user doesn't have enough space in the account.";
    }
};

#endif //FDRIVE_USEREXCEPTION_H
