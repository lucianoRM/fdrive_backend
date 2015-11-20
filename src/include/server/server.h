//
// Created by luciano on 02/09/15.
//

#ifndef SERVER_H_
#define SERVER_H_

#include <string>
#include "mongoose.h"
#include "rocksdb/db.h"
#include <iostream>
#include "user.h"
#include "requestHandler.h"
#include <mutex>

// Class to represent the running server.

class Server {
	private:
		struct mg_server* mongooseServer;
		int listenerTimeOut;

	private:
		// Returns mongoose server.
		struct mg_server* getMongooseServer();

		// Infinite loop polling server.
		static void* infinitePoll(void* server);

	public:
		static std::vector<Server*> serverInstances;
		static bool testing;
		static rocksdb::DB* database;
		static std::mutex autoincrementTransaction;
		// Creates a new server listening in port: port.
		Server(std::string port,rocksdb::DB* database, bool testing);

		// Destroys the server.
		~Server();

		void setListenerTimeOut(int timeOut);

		// Checks the server's connections and handles each one.
		void poll(int timeOut);

		// Creates a new thread where the server will be listening.
		int listenOnThread(void);

		// Copies listeners from server0.
		void copyListeners(Server* server0);

		static int eventHandler(struct mg_connection *conn, enum mg_event ev);

		static int handle_recv(struct mg_connection *conn);
		static int handle_close(struct mg_connection *conn);
};

#endif /* SERVER_H_ */
