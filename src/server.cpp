/*
 * server.cpp
 *
 *  Created on: Sep 2, 2015
 *      Author: luciano
 */

#include "server.h"
#include <iostream>
#include "user.h"


static const char *s_no_cache_header =
  "Cache-Control: max-age=0, post-check=0, "
  "pre-check=0, no-store, no-cache, must-revalidate\r\n";

static void handle_signup_call(struct mg_connection *conn) {
    rocksdb::DB* db;
    rocksdb::Options options;
    options.create_if_missing = true;
    rocksdb::Status status = rocksdb::DB::Open(options, "testdb", &db);
    if (!status.ok()){ std::cout << status.ToString() << std::endl; }

    char email[100], password[100];

	// Get form variables
	mg_get_var(conn, "email", email, sizeof(email));
	mg_get_var(conn, "password", password, sizeof(password));

	User* user = (new User())->setEmail(email)->setPassword(password);
	bool result = user->signup(db);

	mg_printf_data(conn, "{ \"result\": %s }", result ? "true" : "false");

  delete db;
}

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

}

int Server::listenOnThread(void){

    mg_start_thread(infinitePoll,this);
    return 0;

}

void Server::copyListeners(Server* server0){

    mg_copy_listeners(server0->getMongooseServer(),this->mongooseServer);

}



int Server::eventHandler(struct mg_connection *conn, enum mg_event ev) {
  switch (ev) {
    case MG_AUTH: return MG_TRUE;
    case MG_REQUEST:
      if (!strcmp(conn->uri, "/users")) {
        handle_signup_call(conn);
        return MG_TRUE;
      }
      mg_send_file(conn, "index.html", s_no_cache_header);
      return MG_MORE;
    default: return MG_FALSE;
  }
}


