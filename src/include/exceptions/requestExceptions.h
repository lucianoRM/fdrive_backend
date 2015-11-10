#ifndef TALLER_REQUESTEXCEPTIONS_H
#define TALLER_REQUESTEXCEPTIONS_H

#include <iostream>
#include <exception>

// Classes to represent the Exceptions related to Requests.

class RequestException: public std::exception {

    virtual const char* what() const throw() {
        return "Bad request";
    }

};

#endif //TALLER_REQUESTEXCEPTIONS_H
