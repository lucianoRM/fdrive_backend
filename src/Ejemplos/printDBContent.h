#ifndef PRINT_H
#define PRINT_H


#include "../src/include/rocksdb/db.h"
#include "../src/include/rocksdb/status.h"
#include <iostream>

int main2() {
	rocksdb::DB* db;
	rocksdb::Options options;
	rocksdb::Status status = rocksdb::DB::Open(options, "testdb", &db);
	if (!status.ok()){
		//La db no se abrio correctamente o no existía.
		std::cout << "No existe la base de datos o no pudo abrirse." << status.ToString() << std::endl;
		////delete db;
		return 1;
	}
	rocksdb::Iterator* it = db->NewIterator(rocksdb::ReadOptions());
	for (it->SeekToFirst(); it->Valid(); it->Next()) {
		std::cout << it->key().ToString() << ": " << it->value().ToString() << std::endl;
	}
	if (!it->status().ok()) { // Check for any errors found during the scan
		std::cout << "Error al recorrer la DB: " << it->status().ToString() << std::endl;
	}
	delete it;
	////delete db;
	return 0;
}

#endif //PRINT_H
