//
// Created by agustin on 10/09/15.
//

#include <string>
#include "user.h"
#include "rocksdb/db.h"
#include <cstdio>
#include <iostream>

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

bool User::signup(rocksdb::DB* db, std::String email, std::string password){
	std::string value;
    rocksdb::Status status = db->Get(rocksdb::ReadOptions(), "users."+email, &value);
    if (status.IsNotFound() == false)
        return false;
    
	std::string hashed_password = password; //hash(password)
    status = db->Put(rocksdb::WriteOptions(), "users."+email, "{email:"+email+", password:"+hashed_password+"}");
    if (!status.ok()) std::cout << "Error al guardar usuario: " << email << "." << std::endl;
    return true;
}

bool User::checkPassword(rocksdb::DB* db, std::String email, std::string password){
	std::string pass;
    rocksdb::Status status = db->Get(rocksdb::ReadOptions(), "users."+email, &pass);
    if (status.IsNotFound() == true)
        return false;
    
	std::string hashed_password = password; //hash(password)
    if (hashed_password == pass) return true;
    else return false;
}
