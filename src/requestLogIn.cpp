#include "requestLogIn.h"
#include <iostream> ///

int LogIn::counter = 0;

std::string LogIn::createToken(){
	return "token" + std::to_string(counter);
	counter++;
}

void LogIn::attend(struct mg_connection *conn){

	char email[100], password[100];
	// Get form variables
	mg_get_var(conn, "email", email, sizeof(email));
	mg_get_var(conn, "password", password, sizeof(password));
	
	User* user = new User();
	
	rocksdb::DB* db;
	rocksdb::Options options;
	options.create_if_missing = true;
	rocksdb::Status status = rocksdb::DB::Open(options, "testdb", &db);
	if (!status.ok()){ std::cout << "En LogIn: " << status.ToString() << std::endl; }
	bool result = user->load(db, email);
	delete db;
	
	result &= user->checkPassword(password);
	
	std::string token = createToken();
	
	status = rocksdb::DB::Open(options, "testdb", &db);
	if (!status.ok()){ std::cout << "En LogIn: " << status.ToString() << std::endl; }
	result &= user->addToken(db, email, token);
	delete db;

	mg_printf_data(conn, "{ \"result\" : \"%s\" , \"token\" : \"%s\"}", result ? "true" : "false", token.c_str());

	delete user;
	
}
