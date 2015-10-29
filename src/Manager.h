//
// Created by martin on 6/10/15.
//

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
        rocksdb::DB* openDatabase(std::string message);
};

#endif //FDRIVE_MANAGER_H
