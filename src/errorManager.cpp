//
// Created by luciano on 03/10/15.
//

#include "errorManager.h"


ErrorManager::ErrorManager(){

    this->errorMessages = new std::unordered_map<int,std::string>;

    (*this->errorMessages)[LOGIN_FAIL] = std::string("Login Fail");
    (*this->errorMessages)[EMAIL_TAKEN] = std::string("Email Taken");
    (*this->errorMessages)[EXTENSION_NOT_VALID] = std::string("Extension not valid");
    (*this->errorMessages)[NAME_NOT_VALID] = std::string("Name not valid");

}

ErrorManager::~ErrorManager(){

    delete this->errorMessages;

}

std::string ErrorManager::getMessage(int errCode){

    std::string errMessage;
    try{
        errMessage = this->errorMessages->at(errCode);
    }catch(const std::out_of_range& oor){
        return std::string("Unknown Error");
    }

    return errMessage;

}

