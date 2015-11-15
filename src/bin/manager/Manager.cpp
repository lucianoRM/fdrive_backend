#include "Manager.h"


void Manager::setDatabase(rocksdb::DB* db) {
    this->db = db;
}

rocksdb::DB* Manager::openDatabase(std::string message,char openMode) {
    std::cout << "Lo pidieron en: " << message << std::endl;
    /*
    if (Manager::instance != NULL) return &Manager::instance;
    std::cout << "No la devolvió. La va a abrir." << std::endl;
    rocksdb::Options options;
    options.create_if_missing = true;
    rocksdb::Status status;
    //if(openMode == 'r') {
    //    status = rocksdb::DB::OpenForReadOnly(options, "testdb", &Manager::instance);
    //}
    //else {
        status = rocksdb::DB::Open(options, "testdb", Manager::instance);
    //}
    std::cout << "La abrió." << std::endl;
    system("chmod -R a+rwx testdb");

    if (!status.ok()) {
		// The database didn't open correctly.
        std::cout << message << status.ToString() << std::endl;
        delete &Manager::instance;
        Manager::opened = false;
        throw DBException();
    }
    std::cout << "La devolvió." << std::endl;
    return &Manager::instance;
     */
    if (this->db == NULL) std::cout << "Es NULL" << std::endl;
    return this->db;
}
