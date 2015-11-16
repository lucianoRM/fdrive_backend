#ifndef TALLER_ROUTEEXCEPTIONS_H
#define TALLER_ROUTEEXCEPTIONS_H

#include <iostream>
#include <exception>

// Classes to represent the Exceptions related to the routes.

class RouteNotFoundException: public std::exception {

    virtual const char* what() const throw() {
        return "Route not found.";
    }

};

#endif //TALLER_ROUTEEXCEPTIONS_H
