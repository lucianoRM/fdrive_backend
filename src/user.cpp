//
// Created by agustin on 10/09/15.
//

#include <string>
#include "user.h"
#include "rocksdb/db.h"
#include <cstdio>
#include <iostream>
#include "jsoncpp/json/json.h"

bool User::signup(rocksdb::DB* db) {
    std::string value;
    rocksdb::Status status = db->Get(rocksdb::ReadOptions(), "users."+this->email, &value);
    if (status.IsNotFound() == false)
        return false;

    status = db->Put(rocksdb::WriteOptions(), "users."+this->email, "{email:"+this->email+", password:"+this->hashed_password+"}");
    if (!status.ok()) std::cout << "Error al guardar usuario" << std::endl;
    return true;
}

void User::setEmail(std::string email) {
    this->email = email;
}

std::string User::getEmail() {
    return this->email;
}

void User::setPassword(std::string password) {
    this->hashed_password = password;
}

bool User::checkPassword(std::string password) {
    return this->hashed_password == password;
}

bool User::save() {
    return true;
}

User* User::get(std::string email) {
    User* user = new User();
    return user;
}


/* Version 2 */

bool User::signup(rocksdb::DB* db, std::string email, std::string password){
	std::string value;
    rocksdb::Status status = db->Get(rocksdb::ReadOptions(), "users."+email, &value);
    if (status.IsNotFound() == false)
        return false;
    
	std::string hashed_password = password; //hash(password)
    status = db->Put(rocksdb::WriteOptions(), "users."+email, "{email:"+email+", password:"+hashed_password+"}");
    if (!status.ok()) std::cout << "Error al guardar usuario: " << email << "." << std::endl;
    return true;
}

bool User::checkPassword(rocksdb::DB* db, std::string email, std::string password){
	std::string value;
    rocksdb::Status status = db->Get(rocksdb::ReadOptions(), "users."+email, &value);
    if (status.IsNotFound() == true) return false;
    
    Json::Reader reader;
    Json::Value root;
    bool parsingSuccessful = reader.parse(value, root, false); // False for ignoring comments.
    if (!parsingSuccessful){
		std::cout << "JsonCPP no pudo parsear en checkPassword." << std::endl;
		return false;
	}
    std::string pass = root.get("password", "").asString(); // No debería traer errores ni quedar en "" porque se supone que tiene el campo password.
    
	std::string hashed_password = password; //hash(password)
    return (hashed_password == pass);
}

/* También limpia tokens vencidos */
bool User::addToken(rocksdb::DB* db, std::string email, std::string token){
	std::string value;
    rocksdb::Status status = db->Get(rocksdb::ReadOptions(), "users."+email, &value);
    if (status.IsNotFound() == true) return false;
    
    Json::Reader reader;
    Json::Value root;
    bool parsingSuccessful = reader.parse(value, root, false); // False for ignoring comments.
    if (!parsingSuccessful){
		std::cout << "JsonCPP no pudo parsear en addToken." << std::endl;
		return false;
	}
	Json::Value newTokens;
	if (root.isMember("tokens")){
		Json::Value tokens = root["tokens"];
		for (size_t i = 0; i < tokens.size(); i++){
			//if (!viejo)
				newTokens.append(tokens[(int)i]);
		}
	}
	Json::Value jsonToken;
	jsonToken["token"] = token;
	//jsonToken["expiration"] = ...;
	newTokens.append(jsonToken);
	root["tokens"] = newTokens;
	
	Json::StyledWriter writer;
	status = db->Put(rocksdb::WriteOptions(), "users."+email, writer.write(root));
	if (!status.ok()) std::cout << "Error al guardar token nuevo en usuario: " << email << "." << std::endl;
    return true;
	
}
