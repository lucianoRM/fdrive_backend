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
    if (this->load(db, this->email)) return false;
    return this->save(db);
}

bool User::load(rocksdb::DB* db, std::string email) {
	std::string value;
    rocksdb::Status status = db->Get(rocksdb::ReadOptions(), "users."+email, &value);
    if (status.IsNotFound() == false)
        return false;
    
    Json::Reader reader;
    Json::Value root;
    if (!reader.parse(value, root, false)){ // False for ignoring comments.
		std::cout << "JsonCPP no pudo parsear en User::load." << std::endl;
		return false;
	}
	this->email = root.get("email", "").asString();
	this->hashed_password = root.get("password", "").asString();
	//Faltan tokens;
	
    return true;
}

bool User::save(rocksdb::DB* db) {
	rocksdb::Status status = db->Put(rocksdb::WriteOptions(), "users."+this->email, "{email:"+this->email+", password:"+this->hashed_password+"}");
	return (status.ok());	
}

User* User::setEmail(std::string email) {
    this->email = email;
    return this;
}

std::string User::getEmail() {
    return this->email;
}

User* User::setPassword(std::string password) {
    this->hashed_password = this->hashPassword(password);
    return this;
}

User* User::get(std::string email) {
    User* user = new User();
    return user;
}

bool User::checkPassword(std::string password){	
    return (this->hashed_password == this->hashPassword(password));
}

std::string User::hashPassword (std::string password) {
	return password;
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
