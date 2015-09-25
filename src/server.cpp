/*
 * server.cpp
 *
 *  Created on: Sep 2, 2015
 *      Author: luciano
 */

#include "server.h"
#include "mongoose.h"
#include <string.h>
#include <stdio.h>
#include <cstdio>
#include <iostream>
#include "rocksdb/db.h"
#include "user.h"
#include "requestHandler.h"


static const char *s_no_cache_header =
  "Cache-Control: max-age=0, post-check=0, "
  "pre-check=0, no-store, no-cache, must-revalidate\r\n";

Server::Server(std::string port) {

	//Creo el servidor de mongoose/
	mongooseServer = mg_create_server(NULL, this->eventHandler);

	//Defino el listening port

	mg_set_option(mongooseServer, "listening_port", port.c_str());

    printf("Running on port %s\n", port.c_str());
    fflush(stdout);

}

Server::~Server() {
	//Destruyo el servidor de mongoose
	mg_destroy_server(&mongooseServer);
	
}



void Server::poll(int timeOut){
	
	mg_poll_server(mongooseServer,timeOut);

}

int Server::eventHandler(struct mg_connection *conn, enum mg_event ev) {
    RequestHandler* reqHandler = new RequestHandler();
    switch (ev) {
		case MG_AUTH:
			delete reqHandler;
			return MG_TRUE;
		case MG_REQUEST:
			if (reqHandler->handle(std::string(conn->uri), std::string(conn->request_method), conn)){
				delete reqHandler;
				return MG_TRUE;
			}
			mg_send_file(conn, "index.html", s_no_cache_header);
			delete reqHandler;
			return MG_MORE;
		default:
			delete reqHandler;
			return MG_FALSE;
  }
}


