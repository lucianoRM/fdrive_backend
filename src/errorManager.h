//
// Created by luciano on 03/10/15.
//

#ifndef TALLER_ERRORMANAGER_H
#define TALLER_ERRORMANAGER_H

#include <string>
#include <unordered_map>


enum errorCode : int {
    LOGIN_FAIL = 1,
    NOT_LOGGED_IN,
    EMAIL_TAKEN,
    FILEEXTENSION_NOT_VALID,
    FILENAME_NOT_VALID,
    FILENAME_TAKEN,
    FILE_NOT_FOUND,
};



class ErrorManager {

    private:
        std::unordered_map<int,std::string>* errorMessages;

    public:
        ErrorManager();
        ~ErrorManager();

        std::string getMessage(int errCode);



};


#endif //TALLER_ERRORMANAGER_H
