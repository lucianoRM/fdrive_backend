//
// Created by agustin on 14/11/15.
//

#include "routeTreeNode.h"

RouteTreeNode::RouteTreeNode() {
    this->children = new std::map<std::string, RouteTreeNode*>();
}

void RouteTreeNode::addRoute(std::list<std::string>* routeList, int function) {
    if (routeList->empty()) {
        this->data = function;
    } else {
        std::string nextParam = routeList->front();
        routeList->pop_front();
        RouteTreeNode* routeTreeNode = new RouteTreeNode();
        //this->children->insert(std::pair<std::string, RouteTreeNode*> (nextParam, routeTreeNode));
        (*this->children)[nextParam] = routeTreeNode;
        routeTreeNode->addRoute(routeList, function);
    }
}

int RouteTreeNode::getData() { return 0; }

int RouteTreeNode::getFunction(std::list<std::string>* routeList) {
    if (routeList->empty()) {
        //std::cout << "RouteTreeNode::getFunction Return data" << std::endl;
        return this->data;
    }
    std::string nextParam = routeList->front();
    routeList->pop_front();
    if (this->children->find(nextParam) != this->children->end()) {
        //std::cout << "RouteTreeNode::getFunction " << nextParam << " is a child" << std::endl;
        try {
            return this->children->at(nextParam)->getFunction(routeList);
        } catch (RouteNotFoundException e) {std::cout << "RouteTreeNode::getFunction Couldnt fetch from children" << std::endl;}
    }

    if (this->is_int(nextParam) && this->children->find(":int") != this->children->end()) {
        try {
            //std::cout << "RouteTreeNode::getFunction " << nextParam << " is int" << std::endl;
            return this->children->at(":int")->getFunction(routeList);
        } catch (RouteNotFoundException e) {std::cout << "RouteTreeNode::getFunction Couldnt fetch from int" << std::endl;}
    }

    if (this->children->find(":str") != this->children->end()) {
        try {
            //std::cout << "RouteTreeNode::getFunction " << nextParam << " is str" << std::endl;
            return this->children->at(":str")->getFunction(routeList);
        } catch (RouteNotFoundException e) {std::cout << "RouteTreeNode::getFunction Couldnt fetch from str" << std::endl;}
    }

    routeList->push_front(nextParam);
    throw RouteNotFoundException();
}

bool RouteTreeNode::is_int(const std::string& s) {
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}