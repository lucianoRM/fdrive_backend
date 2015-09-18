#include <string.h>
#include "requestHandler.h"

RequestHandler::RequestHandler(){
	rocksdb::Options options;
	options.create_if_missing = true;
	rocksdb::Status status = rocksdb::DB::Open(options, "testdb", &db);
	if (!status.ok()){ std::cout << status.ToString() << std::endl; }
	
	valid_requests = new unordered_map<std::string, Request*>();
	valid_requests.emplace("/users", new AddUser());
	valid_requests.emplace("/login", new LogIn());
}

RequestHandler::~RequestHandler(){
	delete db;
	
	for (&auto pair : valid_requests){
		delete x.second;
	}
	delete valid_requests;
}

bool RequestHandler::handle(std::string uri, std::string request_method, struct mg_connection* conn){
	std::unordered_map<std::string,Request*>::const_iterator got = valid_requests.find(uri+request_method);
	 if ( got == valid_requests.end() ) return false;
	 got->second->attend(conn);
}
