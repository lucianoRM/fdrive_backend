#include "requestLogIn.h"
#include <iostream> ///

int LogIn::counter = 0;

std::string LogIn::createToken(){
	return "token" + std::to_string(counter);
	counter++;
}

void LogIn::attend(struct mg_connection *conn, rocksdb::DB* db){

	char email[100], password[100];
	// Get form variables
	mg_get_var(conn, "email", email, sizeof(email));
	mg_get_var(conn, "password", password, sizeof(password));
	
	User* user = new User();
	bool result = user->load(db, email);
	
	result &= user->checkPassword(password);
	
	std::string token = createToken();
	result &= user->addToken(db, email, token);
	
	mg_printf_data(conn, "{ \"result\" : \"%s\" , \"token\" : \"%s\"}", result ? "true" : "false", token.c_str());

	delete user;
	
}
