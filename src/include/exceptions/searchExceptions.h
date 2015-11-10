#ifndef TALLER_SEARCHEXCEPTIONS_H
#define TALLER_SEARCHEXCEPTIONS_H

#include <exception>

class SearchException: public std::exception {

    virtual const char* what() const throw() {
        return "Error with search";
    }

};

class FileToEraseNotInSearch: public SearchException {

    virtual const char* what() const throw() {
        return "Error with search";
    }

};


#endif //TALLER_SEARCHEXCEPTIONS_H