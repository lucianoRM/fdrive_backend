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

    User* user = new User();
    user->setEmail(email);
    user->setPassword(password);
    bool result = user->signup(db);

    mg_printf_data(conn, "{ \"result\": %s }", result ? "true" : "false");

  delete db;
}

Server::Server(std::string port) {

	//Creo el servidor de mongoose/
	mongooseServer = mg_create_server(NULL, this->eventHandler);

	//Defino el listening port
	mg_set_option(mongooseServer, "listening_port", port.c_str());


}

Server::~Server() {
	//Destruyo el servidor de mongoose
	mg_destroy_server(&mongooseServer);
}



void Server::poll(int timeOut){
	
	mg_poll_server(mongooseServer,timeOut);

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


