#include <unistd.h>
#include "server.h"

#define TOTAL_LISTENERS 5
#define LISTENING_PORT "8000"

int main() {
	rocksdb::DB* db;
	rocksdb::Options options;
	options.create_if_missing = true;
	rocksdb::Status status;

	status = rocksdb::DB::Open(options, "testdb", &db);

	system("chmod -R a+rwx testdb");

	if (!status.ok()) {
		// The database didn't open correctly.
		std::cout << status.ToString() << std::endl;
		////delete db;

	}


	// Creates main server.
	Server* mainServer = new Server(LISTENING_PORT,db);

	// Creates TOTAL_LISTENERS servers to handle up to TOTAL_LISTENERS connections at the same time and launches a thread for each one.
	for (int i = 0; i < TOTAL_LISTENERS; i++) {
		Server* server = new Server(LISTENING_PORT,db);
		server->copyListeners(mainServer);
		server->listenOnThread();
	}

	// Prevents from closing.
	while(true) usleep(10000);

	return 0;
}
