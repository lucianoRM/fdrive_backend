//
// Created by luciano on 03/10/15.
//

#include "errorManager.h"


ErrorManager::ErrorManager(){

    this->errorMessages = new std::unordered_map<int,std::string>;

    (*this->errorMessages)[errorCode::LOGIN_FAIL] = std::string("Login Fail");
    (*this->errorMessages)[errorCode::EMAIL_TAKEN] = std::string("Email Taken");
    (*this->errorMessages)[errorCode::FILEEXTENSION_NOT_VALID] = std::string("File extension not valid");
    (*this->errorMessages)[errorCode::FILENAME_NOT_VALID] = std::string("Filename not valid");
    (*this->errorMessages)[errorCode::FILENAME_TAKEN] = std::string("Filename taken");
    (*this->errorMessages)[errorCode::FILE_NOT_FOUND] = std::string("File not found");
    (*this->errorMessages)[errorCode::NOT_LOGGED_IN] = std::string("Not logged in");
    (*this->errorMessages)[errorCode::INVALID_REQUEST] = std::string("Invalid request");
    (*this->errorMessages)[errorCode::DB_ERROR] = std::string("DB Error");



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

