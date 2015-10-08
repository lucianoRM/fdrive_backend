//
// Created by luciano on 08/10/15.
//

#ifndef TALLER_REQUESTEXCEPTIONS_H
#define TALLER_REQUESTEXCEPTIONS_H

#include <iostream>
#include <exception>


class RequestException: public std::exception{

    virtual const char* what() const throw(){
        return "Bad request";
    }

};



#endif //TALLER_REQUESTEXCEPTIONS_H
