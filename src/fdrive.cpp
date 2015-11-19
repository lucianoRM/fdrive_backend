#include <unistd.h>
#include <sys/stat.h>
#include "server.h"
#include "easylogging/easylogging++.h"

INITIALIZE_EASYLOGGINGPP

#define TOTAL_LISTENERS 5
#define LISTENING_PORT "8000"

int main(int argc, char* argv[]) {
	el::Configurations conf("log.conf");
	el::Loggers::reconfigureAllLoggers(conf);
	LOG(INFO) << "----------------------------------";
	LOG(INFO) << "Starting FDrive";
	rocksdb::DB* db;
	rocksdb::Options options;
	options.create_if_missing = true;
	rocksdb::Status status;

	status = rocksdb::DB::Open(options, "testdb", &db);

	mkdir("files", S_IRWXU | S_IRWXG | S_IRWXO);

	system("chmod -R a+rwx testdb");
	system("chmod -R a+rwx files");

	if (!status.ok()) {
		// The database didn't open correctly.
		LOG(FATAL) << status.ToString();
		////delete db;

	}


	// Creates main server.
	bool testing = false;
	if (argc > 1 && strcmp(argv[1], "test") == 0) {
		LOG(INFO) << "Server is in testing mode.";
		el::Configurations conf("log_test.conf");
		el::Loggers::reconfigureAllLoggers(conf);
		testing = true;
	}
	Server* mainServer = new Server(LISTENING_PORT,db,testing);

	// Creates TOTAL_LISTENERS servers to handle up to TOTAL_LISTENERS connections at the same time and launches a thread for each one.
	LOG(INFO) << "Creating " << TOTAL_LISTENERS << " threads to listen for connections on port " << LISTENING_PORT << ".";
	for (int i = 0; i < TOTAL_LISTENERS; i++) {
		Server* server = new Server(LISTENING_PORT,db,testing);
		server->copyListeners(mainServer);
		server->listenOnThread();
	}

	// Prevents from closing.
	while(true) usleep(10000);

	return 0;
}
