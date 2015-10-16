#include <cstdio>
#include <iostream>
#include <string>

#include "assert.h"
#include "rocksdb/db.h"

using namespace rocksdb;

int main() {
	DB* db;

	Options options;
	options.create_if_missing = true;
	//options.error_if_exists = true;

	Status status = DB::Open(options, "testdb", &db);
	if (!status.ok()){ std::cout << status.ToString() << std::endl; return 1; }

	// Escribir
	status = db->Put(WriteOptions(), "Saludo", "Hola Mundo");
	if (!status.ok()) std::cout << "Error al escribir Saludo" << std::endl;

	//Consultar
	std::string value;
	status = db->Get(ReadOptions(), "Saludo", &value);
	if (!status.ok()) std::cout << "Error al leer Saludo" << std::endl;
	std::cout << "Pido lo que guardé en Saludo: " << value << std::endl;
	
	//Escribir
	value = "Chau Mundo";
	status = db->Put(WriteOptions(), "Adiós", value);
	if (!status.ok()) std::cout << "Error al escribir Adiós" << std::endl;

	//Consultar
	status = db->Get(ReadOptions(), "Saludo", &value);
	if (!status.ok()) std::cout << "Error al leer Saludo2" << std::endl;
	std::cout << "Pido de nuevo Saludo luego de haber usado la misma variable para guardar Adiós: " << value << std::endl;

	//Consultar
	status = db->Get(ReadOptions(), "Adiós", &value);
	if (!status.ok()) std::cout << "Error al leer Saludo2" << std::endl;
	std::cout << "Pido Adiós: " << value << std::endl;

	delete db;

	return 0;
}
