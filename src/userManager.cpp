//
// Created by luciano on 29/09/15.
//

#include "userManager.h"



UserManager::UserManager(){}
UserManager::~UserManager(){}


std::string UserManager::createToken(){

    std::string token = "token" + std::to_string(counter);
    counter++;
    return token;

}




int UserManager::addUser(struct mg_connection *conn){

    char email[100], password[100];
    // Get form variables
    mg_get_var(conn, "email", email, sizeof(email));
    mg_get_var(conn, "password", password, sizeof(password));

    User* user = (new User())->setEmail(email)->setPassword(password);

    rocksdb::DB* db;
    rocksdb::Options options;
    options.create_if_missing = true;
    rocksdb::Status status = rocksdb::DB::Open(options, "testdb", &db);
    if (!status.ok()){ std::cout << "En AddUser:" << status.ToString() << std::endl; }
    bool result = user->signup(db);
    delete db;

    mg_printf_data(conn, "{ \"result\":  \"%s\" }", result ? "true" : "false");

    delete user;
    return 0;

}



int UserManager::userLogin(struct mg_connection *conn){

    char email[100], password[100];
    // Get form variables
    mg_get_var(conn, "email", email, sizeof(email));
    mg_get_var(conn, "password", password, sizeof(password));

    User* user = new User();

    rocksdb::DB* db;
    rocksdb::Options options;
    options.create_if_missing = true;
    rocksdb::Status status = rocksdb::DB::Open(options, "testdb", &db);
    if (!status.ok()){ std::cout << "En LogIn: " << status.ToString() << std::endl; }
    bool result = user->load(db, email);
    delete db;

    result &= user->checkPassword(password);

    std::string token = createToken();

    status = rocksdb::DB::Open(options, "testdb", &db);
    if (!status.ok()){ std::cout << "En LogIn: " << status.ToString() << std::endl; }
    result &= user->addToken(db, email, token);
    delete db;

    mg_printf_data(conn, "{ \"result\" : \"%s\" , \"token\" : \"%s\"}", result ? "true" : "false", token.c_str());

    delete user;
    return 0;

}