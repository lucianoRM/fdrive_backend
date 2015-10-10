/*
 * server.cpp
 *
 *  Created on: Sep 2, 2015
 *      Author: luciano
 */

#include "server.h"
#include <iostream>
#include "user.h"
#include "requestHandler.h"


static const char *s_no_cache_header =
  "Cache-Control: max-age=0, post-check=0, "
  "pre-check=0, no-store, no-cache, must-revalidate\r\n";

Server::Server(std::string port) {

	//Creates mongoose's server
	mongooseServer = mg_create_server(NULL, this->eventHandler);

	//Defines listening port
	mg_set_option(mongooseServer, "listening_port", port.c_str());

    this->listenerTimeOut = 1000;

    printf("Running on port %s\n", port.c_str());
    fflush(stdout);

}

Server::~Server() {
	//Destroys mongoose's server
	mg_destroy_server(&mongooseServer);
	
}


void Server::setListenerTimeOut(int timeOut){

    this->listenerTimeOut = timeOut;

}



struct mg_server* Server::getMongooseServer(){

    return mongooseServer;

}


void Server::poll(int timeOut){
	
	mg_poll_server(mongooseServer,timeOut);

}


//Parameters are void* because function is called in thread.
void* Server::infinitePoll(void* server){

    while(true) ((Server*)server)->poll(((Server*)server)->listenerTimeOut);
	return (void*) -1;

}

int Server::listenOnThread(void){

    mg_start_thread(infinitePoll,this);
    return 0;

}

void Server::copyListeners(Server* server0){

    mg_copy_listeners(server0->getMongooseServer(),this->mongooseServer);

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


