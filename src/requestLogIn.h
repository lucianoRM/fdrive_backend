#include "mongoose.h"
#include "request.h"
#include "user.h"
#include <string>

class LogIn : public Request {

	private:
		static int counter;
		std::string createToken();
	public:
		void attend(struct mg_connection *conn);
		~LogIn(){};
		
};
