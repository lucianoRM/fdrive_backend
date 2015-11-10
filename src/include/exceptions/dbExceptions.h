#ifndef TALLER_DBEXCEPTIONS_H
#define TALLER_DBEXCEPTIONS_H

#include <iostream>
#include <exception>

// Classes to represent the Exceptions related to the Database.

class DBException: public std::exception {

    virtual const char* what() const throw() {
        return "Error with database";
    }

};

#endif //TALLER_DBEXCEPTIONS_H
