#include <string.h>
#include "requestHandler.h"
#include "requestAddUser.h"
#include "requestLogIn.h"
#include <iostream> ///

RequestHandler::RequestHandler(){	
	valid_requests = new std::unordered_map<std::string, Request*>();
	valid_requests->emplace("/users", new AddUser());
	valid_requests->emplace("/login", new LogIn());
}

RequestHandler::~RequestHandler(){
	for (auto pair : *valid_requests){
		delete pair.second;
	}
	delete valid_requests;
}

bool RequestHandler::handle(std::string uri, std::string request_method, struct mg_connection* conn){
	//std::cout << "Request method: " << request_method << std::endl;
	std::unordered_map<std::string,Request*>::const_iterator got = valid_requests->find(uri); //(uri+request_method);
	if ( got == valid_requests->end() ) return false;
	got->second->attend(conn);
	return true;
}
