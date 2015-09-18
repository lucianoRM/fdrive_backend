#include "mongoose.h"
#include "request.h"

class LogIn : public Request {

	public:
		void attend(struct mg_connection *conn, rocksdb::DB* db){

			char email[100], password[100];
			// Get form variables
			mg_get_var(conn, "email", email, sizeof(email));
			mg_get_var(conn, "password", password, sizeof(password));
			
			bool result = User::checkPassword(db, email, password)
			/* Faltaria lo del token. */
			mg_printf_data(conn, "{ \"result\": %s }", result ? "true" : "false");

			delete db;

		}

}
