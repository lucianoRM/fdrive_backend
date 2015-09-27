#ifndef REQUEST_H_
#define REQUEST_H_

#include "mongoose.h"

class Request {

	public:
		virtual void attend(struct mg_connection *conn) = 0;
		virtual ~Request(){}

};

#endif /* REQUEST_H_ */
