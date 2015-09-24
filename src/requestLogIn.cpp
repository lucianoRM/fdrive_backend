#include "mongoose.h"
#include "request.h"
#include <string>

class LogIn : public Request {

	private:
		int counter = 1;
		
		std::string createToken(){
			return "token" + std::string(itoa(this->counter));
			this->counter++;
		}

	public:
		void attend(struct mg_connection *conn, rocksdb::DB* db){

			char email[100], password[100];
			// Get form variables
			mg_get_var(conn, "email", email, sizeof(email));
			mg_get_var(conn, "password", password, sizeof(password));
			
			User* user = User::load(db, email);
			bool result;
			if (user == null) {
				result = false;
			} else {
				result = User::checkPassword(password);
				
				std::string token = createToken();
				result &= user->addToken(token);
			}
			
			mg_printf_data(conn, "{ \"result\": %s }", result ? "true" : "false");

			delete db;

		}

};
