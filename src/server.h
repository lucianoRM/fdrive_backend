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

	public:
		//Crea un nuevo servidor en el puerto port
		Server(std::string port);

		//Destruye el servidor
		~Server();

		//Checkea las conexiones que tiene el servidor y las atiende ya sea para enviar o recibir datos.
		void poll(int timeOut);
	
		static int eventHandler(struct mg_connection *conn, enum mg_event ev);

};


#endif /* SERVER_H_ */
