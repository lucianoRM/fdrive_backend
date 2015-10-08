//
// Created by luciano on 08/10/15.
//

#ifndef TALLER_DBEXCEPTIONS_H
#define TALLER_DBEXCEPTIONS_H

#include <iostream>
#include <exception>

//Default message
class DBException: public std::exception{

    virtual const char* what() const throw(){
        return "Error with database";
    }

};




#endif //TALLER_DBEXCEPTIONS_H
