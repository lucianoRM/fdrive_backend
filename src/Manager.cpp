//
// Created by martin on 6/10/15.
//

#include "Manager.h"
#include <iostream>

rocksdb::DB* Manager::openDatabase(std::string message) {
    rocksdb::DB* db;
    rocksdb::Options options;
    options.create_if_missing = true;
    rocksdb::Status status = rocksdb::DB::Open(options, "testdb", &db);

    //La db se abrio correctamente
    if (!status.ok()){
        std::cout << message << status.ToString() << std::endl;
        delete db;
        return NULL;
    }

    return db;
}