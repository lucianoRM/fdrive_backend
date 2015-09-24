#include "mongoose.h"
#include "rocksdb/db.h"
#include "request.h"
#include "user.h"
#include <string>

class LogIn : public Request {

	private:
		int counter = 1;
		
		std::string createToken();
	public:
		void attend(struct mg_connection *conn, rocksdb::DB* db);
		~LogIn(){};
		
};
