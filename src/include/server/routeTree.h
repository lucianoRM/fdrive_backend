//
// Created by agustin on 14/11/15.
//

#ifndef FDRIVE_ROUTETREE_H
#define FDRIVE_ROUTETREE_H

#include "routeExceptions.h"
#include <map>
#include <list>
#include <vector>
#include "routeTreeNode.h"

class RouteTree {
private:
    std::map<std::string, RouteTreeNode*>* tree;
public:
    RouteTree();
    ~RouteTree();
    void add(std::string route, std::string method, int function);
    int get(std::string route, std::string method);
    std::list<std::string>* getRouteParameterList(std::string route);
    std::vector<std::string>* getRouteParameterVector(std::string route);
};

#endif //FDRIVE_ROUTETREE_H
