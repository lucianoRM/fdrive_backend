#include "server.h"

int main(){

	//Crea el servidor
	Server* server = new Server("9500");
	
	//Itera infinitamente y checkea las conexiones con un timeout de 1  segundo(1000 mS)
	for(;;) server->poll(1000);


	return 0;
}
