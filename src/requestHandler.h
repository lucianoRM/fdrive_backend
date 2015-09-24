#ifndef REQHANDLER_H_
#define REQHANDLER_H_

#include <string.h>
#include <unordered_map>
#include "rocksdb/db.h"
#include "mongoose.h"
#include "request.h"



class RequestHandler {
	private:
		rocksdb::DB* db;
		std::unordered_map<std::string, Request*>* valid_requests;

	public:
	
		RequestHandler();

		~RequestHandler();

		// Devuelve true si era una request válida (más allá de su resultado)
		// o false sino.
		bool handle(std::string uri, std::string request_method, struct mg_connection* conn);

};


#endif /* REQHANDLER_H_ */
