#include <string.h>
#include "requestHandler.h"
#include "requestAddUser.h"
#include "requestLogIn.h"
#include <iostream>

RequestHandler::RequestHandler(){
	rocksdb::Options options;
	options.create_if_missing = true;
	rocksdb::Status status = rocksdb::DB::Open(options, "testdb", &db);
	if (!status.ok()){ std::cout << status.ToString() << std::endl; }
	
	valid_requests = new std::unordered_map<std::string, Request*>();
	valid_requests->emplace("/users", new AddUser());
	valid_requests->emplace("/login", new LogIn());
}

RequestHandler::~RequestHandler(){
	delete db;
	
	//~ for (std::pair<std::string, Request*> pair : valid_requests){
		//~ delete x.second;
	//~ }
	for (auto pair : *valid_requests){
		delete pair.second;
	}
	delete valid_requests;
}

bool RequestHandler::handle(std::string uri, std::string request_method, struct mg_connection* conn){
	std::unordered_map<std::string,Request*>::const_iterator got = valid_requests->find(uri); //(uri+request_method);
	 if ( got == valid_requests->end() ) return false;
	 got->second->attend(conn, db);
	 return true;
}
