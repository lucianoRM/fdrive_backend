#include <string.h>
#include "requestHandler.h"
#include <iostream> ///

RequestHandler::RequestHandler(){

	this->userManager = new UserManager();
	this->fileManager = new FileManager();
	this->errorManager = new ErrorManager();

	this->codesMap = new std::unordered_map<std::string,int>;

	(*this->codesMap)["/users:GET"] = requestCodes::USERS_POST;
	(*this->codesMap)["/login:GET"] = requestCodes::LOGIN_GET;
	(*this->codesMap)["/files:GET"] = requestCodes::SAVEFILE_POST; //TODO:Should be POST, easier to test this way.





}

RequestHandler::~RequestHandler(){

	delete this->codesMap;
	delete this->userManager;
	delete this->fileManager;
	delete this->errorManager;
}

bool RequestHandler::handle(std::string uri, std::string request_method, struct mg_connection* conn) {

	//Combine uri+request_method
	std::string uriPlusMethod = uri + ":" + request_method;
	int reqCode;
	try {
		reqCode = codesMap->at(uriPlusMethod);
	}catch(const std::out_of_range& oor){ //Error needed for checking if uri was found.
		return false;
	}

	try {
		switch (reqCode) {

			//addUser
			case requestCodes::USERS_POST:
				this->userManager->addUser(conn);
				break;

			case requestCodes::LOGIN_GET:
				this->userManager->userLogin(conn);
				break;

			case requestCodes::SAVEFILE_POST:
				this->fileManager->addFile(conn);
				break;
			default:
				return false;
		}
	}catch (errorCode e) {
		mg_printf_data(conn, "%s", errorManager->getMessage(e).c_str()); //Even if there is an error, it shoul return true to close the connection
	}
	return true;
}
