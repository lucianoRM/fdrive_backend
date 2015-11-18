//
// Created by martin on 6/10/15.
//

#include "Manager.h"


Manager::Manager(rocksdb::DB* database){
    this->database = database;


}

rocksdb::DB* Manager::openDatabase(std::string message,char openMode) {
    /*rocksdb::DB* db;
    rocksdb::Options options;
    options.create_if_missing = true;
    rocksdb::Status status;
    if(openMode == 'r') {
        status = rocksdb::DB::OpenForReadOnly(options, "testdb", &db);
    }
    else {
        status = rocksdb::DB::Open(options, "testdb", &db);
    }
    system("chmod -R a+rwx testdb");

    if (!status.ok()) {
		// The database didn't open correctly.
        std::cout << message << status.ToString() << std::endl;
        ////delete db;
        throw DBException();
    }*/

    return this->database;
}
