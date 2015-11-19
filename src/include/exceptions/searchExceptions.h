#ifndef TALLER_SEARCHEXCEPTIONS_H
#define TALLER_SEARCHEXCEPTIONS_H

#include <exception>

class SearchException: public std::exception {

    virtual const char* what() const throw() {
        return "Error with search";
    }

};

class FileNotInSearch: public SearchException {

    virtual const char* what() const throw() {
        return "Error with search, expected file not found in search.";
    }

};


#endif //TALLER_SEARCHEXCEPTIONS_H