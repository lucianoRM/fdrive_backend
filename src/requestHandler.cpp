#include <string.h>
#include "requestHandler.h"
#include <iostream> ///

RequestHandler::RequestHandler(){

	this->userManager = new UserManager();
	this->fileManager = new FileManager();

	this->codesMap = new std::unordered_map<std::string,int>;

	(*this->codesMap)["/users:GET"] = requestCodes::USERS_POST;
	(*this->codesMap)["/login:GET"] = requestCodes::LOGIN_GET;
	(*this->codesMap)["/files:GET"] = requestCodes::SAVEFILE_POST; //TODO:Should be POST, easier to test this way.





}

RequestHandler::~RequestHandler(){

	delete this->codesMap;
	delete this->userManager;
	delete this->fileManager;
}

bool RequestHandler::handle(std::string uri, std::string request_method, struct mg_connection* conn) {

	//Combine uri+request_method
	std::string uriPlusMethod = uri + ":" + request_method;

	int reqCode = (*codesMap)[uriPlusMethod];

	switch (reqCode) {

		//addUser
		case USERS_POST:
			this->userManager->addUser(conn);
			break;

		case LOGIN_GET:
			this->userManager->userLogin(conn);
			break;

		case SAVEFILE_POST:
			this->fileManager->addFile(conn);
			break;
		default:
			return false;
	}

	return true;
}
