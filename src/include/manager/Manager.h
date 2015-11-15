#ifndef FDRIVE_MANAGER_H
#define FDRIVE_MANAGER_H

#include "rocksdb/db.h"
#include "rocksdb/options.h"
#include "rocksdb/status.h"
#include <string>
#include "dbExceptions.h"
#include <iostream>

class Manager {

    protected:
        rocksdb::DB* openDatabase(std::string message,char openMode);
        rocksdb::DB* db = NULL;

    public:
        void setDatabase(rocksdb::DB* db);
};

#endif //FDRIVE_MANAGER_H
