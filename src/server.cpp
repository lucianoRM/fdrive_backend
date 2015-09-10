/*
 * server.cpp
 *
 *  Created on: Sep 2, 2015
 *      Author: luciano
 */

#include "server.h"
#include "mongoose.h"
#include <string.h>


static const char *s_no_cache_header =
  "Cache-Control: max-age=0, post-check=0, "
  "pre-check=0, no-store, no-cache, must-revalidate\r\n";

static void handle_restful_call(struct mg_connection *conn) {
  char n1[100], n2[100];

  // Get form variables
  mg_get_var(conn, "n1", n1, sizeof(n1));
  mg_get_var(conn, "n2", n2, sizeof(n2));

  mg_printf_data(conn, "{ \"result\": %lf }", strtod(n1, NULL) + strtod(n2, NULL));
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
      if (!strcmp(conn->uri, "/api/sum")) {
        handle_restful_call(conn);
        return MG_TRUE;
      }
      mg_send_file(conn, "index.html", s_no_cache_header);
      return MG_MORE;
    default: return MG_FALSE;
  }
}


