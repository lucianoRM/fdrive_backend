#include "mongoose.h"
#include "request.h"

class AddUser : public Request {

	public:
		void attend(struct mg_connection *conn, rocksdb::DB* db){

			char email[100], password[100];
			// Get form variables
			mg_get_var(conn, "email", email, sizeof(email));
			mg_get_var(conn, "password", password, sizeof(password));

			User* user = new User();
			user->setEmail(email);
			user->setPassword(password);
			bool result = user->signup(db);

			mg_printf_data(conn, "{ \"result\": %s }", result ? "true" : "false");

		}

}
