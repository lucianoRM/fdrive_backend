//
// Created by martin on 6/10/15.
//

#include <sys/stat.h>
#include "Manager.h"

rocksdb::DB* Manager::openDatabase(std::string message) {
    rocksdb::DB* db;
    rocksdb::Options options;
    options.create_if_missing = true;
    rocksdb::Status status = rocksdb::DB::Open(options, "testdb", &db);
    system("chmod -R a+rwx testdb");

    if (!status.ok()){
		//La db no se abrio correctamente
        std::cout << message << status.ToString() << std::endl;
        delete db;
        throw DBException();
    }

    return db;
}
