/*
 * server.h
 *
 *  Created on: Sep 2, 2015
 *      Author: luciano
 */

#include <string>
#include "mongoose.h"
#include "rocksdb/db.h"


#ifndef SERVER_H_
#define SERVER_H_

class Server {
	private:
		struct mg_server* mongooseServer;
		int listenerTimeOut;

	private:
		//Returns mongoose server
		struct mg_server* getMongooseServer();

		//Infinite loop polling server
		static void* infinitePoll(void* server);

	public:
		//Creates a new server listening in port: port.
		Server(std::string port);

		//Destroys the server
		~Server();

		void setListenerTimeOut(int timeOut);

		//Checks the server's connections and handles each one.
		void poll(int timeOut);

		//Creates a new thread where the server will be listening
		int listenOnThread(void);

		//Copies listeners from server0
		void copyListeners(Server* server0);

		static int eventHandler(struct mg_connection *conn, enum mg_event ev);

};


#endif /* SERVER_H_ */
