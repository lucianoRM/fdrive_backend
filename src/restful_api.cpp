#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mongoose.h"
#include <cstdio>
#include <iostream>

#include "rocksdb/db.h"

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

static int ev_handler(struct mg_connection *conn, enum mg_event ev) {
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

int main(void) {
  struct mg_server *server;

  // Create and configure the server
  server = mg_create_server(NULL, ev_handler);
  mg_set_option(server, "listening_port", "8000");

  // Example rocksdb
  rocksdb::DB* db;

  rocksdb::Options options;
  options.create_if_missing = true;
  //options.error_if_exists = true;

  rocksdb::Status status = rocksdb::DB::Open(options, "testdb", &db);
  if (!status.ok()){ std::cout << status.ToString() << std::endl; return 1; }

  // Escribir
  status = db->Put(rocksdb::WriteOptions(), "Saludo", "Hola Mundo");
  if (!status.ok()) std::cout << "Error al escribir Saludo" << std::endl;

  //Consultar
  std::string value;
  status = db->Get(rocksdb::ReadOptions(), "Saludo", &value);
  if (!status.ok()) std::cout << "Error al leer Saludo" << std::endl;
  std::cout << "Pido lo que guardé en Saludo: " << value << std::endl;
  // Example rocksdb

  // Serve request. Hit Ctrl-C to terminate the program
  printf("Starting on port %s\n", mg_get_option(server, "listening_port"));
  for (;;) {
    mg_poll_server(server, 1000);
  }

  // Cleanup, and free server instance
  mg_destroy_server(&server);

  return 0;
}
