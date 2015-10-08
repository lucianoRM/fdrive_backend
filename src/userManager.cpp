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

    User* user = new User(email);

    rocksdb::DB* db = this->openDatabase("En AddUser: ");

    bool result = true;

    user->signup(db,password);
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


    User* user = new User(email);


    rocksdb::DB* db = this->openDatabase("En LogIn: ");

    bool result;
    user->load(db,password);
    delete db;

   // result &= user->checkPassword(password);

    std::string token = createToken();

    db = this->openDatabase("En LogIn: ");

    result = user->addToken(db, token);

    delete db;

    mg_printf_data(conn, "{ \"result\" : \"%s\" , \"token\" : \"%s\"}", result ? "true" : "false", token.c_str());

    delete user;
    return 0;

}


void UserManager::checkIfLoggedIn(struct mg_connection* conn){

    //Variable retrieval is different acconding to method
    char cemail[100],ctoken[100];
    std::string email,token;
    if(!strcmp(conn->request_method,"GET")){
        mg_get_var(conn, "email", cemail, sizeof(cemail));
        mg_get_var(conn, "token", ctoken, sizeof(ctoken));
        email = std::string(cemail);
        token = std::string(ctoken);
    }
    else{
        //Needed for filtering unnecesary headers
        char json[conn->content_len+1];
        char* content = conn->content;
        content[conn->content_len] = '\0';
        strcpy(json,conn->content);

        Json::Reader reader;
        Json::Value root;

        reader.parse(json,root,false);

        std::string name,extension,owner;
        if(!root.isMember("email") || !root.isMember("token")) throw RequestException();
        email = root["email"].asString();
        token = root["token"].asString();
    }
    User* user = new User(email);
    rocksdb::DB* db = openDatabase("En check if logged in");

    user->checkToken(db,token);
    delete db;


}
