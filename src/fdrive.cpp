#include <unistd.h>
#include "server.h"

#define TOTAL_LISTENERS 100
#define LISTENING_PORT "8000"


int main(){


	//Creates main server

	Server* mainServer = new Server(LISTENING_PORT);

	//Creates TOTAL_LISTENERS servers to handle up to TOTAL_LISTENERS connections at the same time and launches a thread for each one.
	for(int i = 0; i < TOTAL_LISTENERS; i++){
		Server* server = new Server(LISTENING_PORT);
		server->copyListeners(mainServer);
		server->listenOnThread();
	}

	//Prevents from closing
	while(true) usleep(10000);

	return 0;
}
