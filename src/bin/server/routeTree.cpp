//
// Created by agustin on 14/11/15.
//

#include "routeTree.h"

RouteTree::RouteTree() {
    this->tree = new std::map<std::string, RouteTreeNode*>();
}

RouteTree::~RouteTree() {

}

void RouteTree::add(std::string route, std::string method, int function) {
    if (this->tree->find(method) == this->tree->end())
        (*this->tree)[method] = new RouteTreeNode();
    (*this->tree)[method]->addRoute(this->getRouteParameterList(route), function);
}

int RouteTree::get(std::string route, std::string method) {
    //std::cout << "RouteTree::get " << route << ":" << method << std::endl;
    if (this->tree->find(method) == this->tree->end()) {
        //std::cout << "RouteTree::get ROUTENOTFOUND" << std::endl;
        throw RouteNotFoundException();
    }

    return this->tree->at(method)->getFunction(this->getRouteParameterList(route));
}

std::list<std::string>* RouteTree::getRouteParameterList(std::string route) {
    std::list<std::string>* parameterList = new std::list<std::string>();
    std::string delimiter = "/";
    size_t pos = 0;
    std::string token;
    std::string routeCopy = route;
    while ((pos = routeCopy.find(delimiter)) != std::string::npos) {
        token = routeCopy.substr(0, pos);
        //std::cout << "adding " << token << std::endl;
        parameterList->push_back(token);
        routeCopy.erase(0, pos + delimiter.length());
    }
    //std::cout << "adding " << routeCopy << std::endl;
    parameterList->push_back(routeCopy);
    return parameterList;
}