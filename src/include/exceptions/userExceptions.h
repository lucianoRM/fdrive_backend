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
	private:
		std::string user;
		
	public:
		NotEnoughQuota(std::string user) {
			this->user = user;
		}
		
		virtual const char* what() const throw() {
			return ("The user " + this->user + " doesn't have enough space in the account.").c_str();
		}
};

#endif //FDRIVE_USEREXCEPTION_H
