#include "requestAddUser.h"
#include "user.h"
#include <iostream> ///

void AddUser::attend(struct mg_connection *conn){
	
	char email[100], password[100];
	// Get form variables
	mg_get_var(conn, "email", email, sizeof(email));
	mg_get_var(conn, "password", password, sizeof(password));

	User* user = (new User())->setEmail(email)->setPassword(password);
	
	rocksdb::DB* db;
	rocksdb::Options options;
	options.create_if_missing = true;
	rocksdb::Status status = rocksdb::DB::Open(options, "testdb", &db);
	if (!status.ok()){ std::cout << "En AddUser:" << status.ToString() << std::endl; }
	bool result = user->signup(db);
	delete db;
	
	mg_printf_data(conn, "{ \"result\":  \"%s\" }", result ? "true" : "false");

	delete user;
	
}
