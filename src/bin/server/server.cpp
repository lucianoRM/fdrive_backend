#include "server.h"

static const char *s_no_cache_header =
  "Cache-Control: max-age=0, post-check=0, "
  "pre-check=0, no-store, no-cache, must-revalidate\r\n";

Server::Server(std::string port) {
	// Creates mongoose's server.
	mongooseServer = mg_create_server(NULL, this->eventHandler);

	// Defines listening port.
	mg_set_option(mongooseServer, "listening_port", port.c_str());

    this->listenerTimeOut = 1000;

    printf("Running on port %s\n", port.c_str());
    fflush(stdout);
}

Server::~Server() {
	// Destroys mongoose's server.
	mg_destroy_server(&mongooseServer);
}

void Server::setListenerTimeOut(int timeOut) {
    this->listenerTimeOut = timeOut;
}

struct mg_server* Server::getMongooseServer() {
    return mongooseServer;
}

void Server::poll(int timeOut) {
	mg_poll_server(mongooseServer,timeOut);
}

// Parameters are void* because function is called in thread.
void* Server::infinitePoll(void* server) {
    while (true) {
		((Server*)server)->poll(((Server*)server)->listenerTimeOut);
	}
}

int Server::listenOnThread(void) {
    mg_start_thread(infinitePoll,this);
    return 0;
}

void Server::copyListeners(Server* server0) {
    mg_copy_listeners(server0->getMongooseServer(),this->mongooseServer);
}

int Server::eventHandler(struct mg_connection *conn, enum mg_event ev) {
    RequestHandler* reqHandler = new RequestHandler();
	int result;
    switch (ev) {
		case MG_AUTH:
			delete reqHandler;
			return MG_TRUE;
		case MG_REQUEST:
			result = reqHandler->handle(std::string(conn->uri), std::string(conn->request_method), conn);
		std::cout << result << std::endl;
			if (result == -1) {
				mg_send_file(conn, "index.html", s_no_cache_header);
				delete reqHandler;
				return MG_MORE;
			} else if (result == -2) {
				std::cout << "Poniendo MG_TRUE" << std::endl;
				delete reqHandler;
				return MG_TRUE;
			} else {
				delete reqHandler;
				return result;
			}

		/*case MG_RECV:
			delete reqHandler;
			return handle_recv(conn);
		case MG_CLOSE:
			delete reqHandler;
			return handle_close(conn);*/
		default:
			delete reqHandler;
			return MG_FALSE;
	}
}

// Mongoose sends MG_RECV for every received POST chunk.
// When last POST chunk is received, Mongoose sends MG_REQUEST, then MG_CLOSE.
int Server::handle_recv(struct mg_connection *conn) {
	std::cout << "RECV" << std::endl;
	FILE *fp = (FILE *) conn->connection_param;

	// Open temporary file where we going to write data
	if (fp == NULL && ((conn->connection_param = fp = tmpfile())) == NULL) {
		return -1;  // Close connection on error
	}

	// Return number of bytes written to a temporary file: that is how many
	// bytes we want to discard from the receive buffer
	return fwrite(conn->content, 1, conn->content_len, fp);
}

// Make sure we free all allocated resources
int Server::handle_close(struct mg_connection *conn) {
	std::cout << "CLOSE" << std::endl;
	if (conn->connection_param != NULL) {
		fclose((FILE *) conn->connection_param);
		conn->connection_param = NULL;
	}
	return MG_TRUE;
}