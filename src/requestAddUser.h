#include "mongoose.h"
#include "rocksdb/db.h"
#include "request.h"

class AddUser : public Request {

	public:
		void attend(struct mg_connection *conn);
		~AddUser(){}

};
