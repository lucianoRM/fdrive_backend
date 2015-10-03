//
// Created by luciano on 03/10/15.
//

#ifndef TALLER_ERRORMANAGER_H
#define TALLER_ERRORMANAGER_H

#include <string>
#include <unordered_map>


enum errorCode : int {
    LOGIN_FAIL = 1,
    EMAIL_TAKEN,
    EXTENSION_NOT_VALID,
    NAME_NOT_VALID,
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
