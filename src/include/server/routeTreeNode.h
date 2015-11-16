//
// Created by agustin on 14/11/15.
//

#ifndef FDRIVE_ROUTETREENODE_H
#define FDRIVE_ROUTETREENODE_H

#include "routeExceptions.h"
#include <map>
#include <list>

class RouteTreeNode {
private:
    int data = 0;
    std::map<std::string, RouteTreeNode*>* children;
    bool is_int(const std::string& s);
public:
    RouteTreeNode();
    ~RouteTreeNode();
    int getData();
    int getFunction(std::list<std::string>* routeList);
    void addRoute(std::list<std::string>* routeList, int function);
};

#endif //FDRIVE_ROUTETREENODE_H
