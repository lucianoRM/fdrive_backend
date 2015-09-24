#include "requestAddUser.h"
#include "user.h"

void AddUser::attend(struct mg_connection *conn, rocksdb::DB* db){

	char email[100], password[100];
	// Get form variables
	mg_get_var(conn, "email", email, sizeof(email));
	mg_get_var(conn, "password", password, sizeof(password));

	User* user = (new User())->setEmail(email)->setPassword(password);
	bool result = user->signup(db);

	mg_printf_data(conn, "{ \"result\": %s }", result ? "true" : "false");

}
