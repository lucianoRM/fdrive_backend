#include <unistd.h>
#include <sys/stat.h>
#include "server.h"
#include "easylogging/easylogging++.h"

INITIALIZE_EASYLOGGINGPP

#define TOTAL_LISTENERS 5
#define LISTENING_PORT "8000"

extern "C" void __gcov_flush();

void signalHandler( int signum )
{
	std::cout << "called signal handler";
	__gcov_flush();
	for ( unsigned long i = 0; i < Server::serverInstances.size(); i++ )
	{
		delete Server::serverInstances[i];
	}
	/*for (std::vector<Server*>::iterator it = Server::serverInstances.begin(); it != Server::serverInstances.end(); it++) {
		std::vector<Server*>::iterator currentElement = it;
		it++;
		delete *currentElement;
	}*/

	exit(signum);

}

int main(int argc, char* argv[]) {
	signal(SIGTERM, signalHandler);
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
	Server::serverInstances.push_back(mainServer);

	// Creates TOTAL_LISTENERS servers to handle up to TOTAL_LISTENERS connections at the same time and launches a thread for each one.
	LOG(INFO) << "Creating " << TOTAL_LISTENERS << " threads to listen for connections on port " << LISTENING_PORT << ".";
	//Server::serverInstances = new std::vector<Server*>();
	//mainServer->serverInstances = new std::vector<Server*>();
	for (int i = 0; i < TOTAL_LISTENERS; i++) {
		Server* server = new Server(LISTENING_PORT,db,testing);
		server->copyListeners(mainServer);
		server->listenOnThread();
		//mainServer->serverInstances->push_back(server);
		Server::serverInstances.push_back(server);
	}

	// Prevents from closing.
	while(true) usleep(10000);

	return 0;
}
