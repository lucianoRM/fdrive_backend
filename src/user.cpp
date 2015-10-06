//
// Created by agustin on 10/09/15.
//

#include "user.h"
#include <iostream>
#include "json/json.h"
#include "json/json-forwards.h"


User::User(std::string email, std::string password) {
	this->email = email;
	this->hashed_password = this->hashPassword(password);
}

bool User::signup(rocksdb::DB* db) {
    if (this->load(db)) return false;
    return this->save(db);
}

bool User::load(rocksdb::DB* db) {
	std::string value;
    rocksdb::Status status = db->Get(rocksdb::ReadOptions(), "users."+email, &value);
    if (status.IsNotFound()) return false;
    
    Json::Reader reader;
    Json::Value root;
    bool parsingSuccessful = reader.parse(value, root, false);
    if (!parsingSuccessful){ // False for ignoring comments.
		std::cout << "JsonCPP no pudo parsear en User::load. Value: " << value << ". root: " << root << std::endl;
		return false;
	}
	this->email = root.get("email", "").asString();
	this->hashed_password = root.get("password", "").asString();
	//Faltan tokens;
	
    return true;
}

bool User::save(rocksdb::DB* db) {
	rocksdb::Status status = db->Put(rocksdb::WriteOptions(), "users."+this->email, "{\"email\":\""+this->email+"\", \"password\":\""+this->hashed_password+"\"}");
	return (status.ok());	
}

std::string User::getEmail() {
    return this->email;
}

void User::setPassword(std::string password) {
    this->hashed_password = this->hashPassword(password);
}

bool User::checkPassword(std::string password){	
    return (this->hashed_password == this->hashPassword(password));
}

std::string User::hashPassword (std::string password) {
	return password;
}

/* También limpia tokens vencidos */
bool User::addToken(rocksdb::DB* db, std::string token){
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
	jsonToken["token"] = "\"" + token + "\"";
	//jsonToken["expiration"] = ...;
	newTokens.append(jsonToken);
	root["tokens"] = newTokens;
	
	Json::StyledWriter writer;
	status = db->Put(rocksdb::WriteOptions(), "users."+email, writer.write(root));
	if (!status.ok()){
		std::cout << "Error al guardar token nuevo en usuario: " << email << "." << std::endl;
		return false;
	}
    return true;
	
}
