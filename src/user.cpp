#include "user.h"
#include "UserException.h"
#include "json/json.h"
#include "json/json-forwards.h"
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/engine.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <iostream> ///

User::User(){
	this->tokens = new std::vector<UserToken*>();
}

User::~User(){
	for (UserToken* token : *this->tokens) {
		delete token;
	}
	delete this->tokens;
}

std::string User::getEmail() {
	return this->email;
}

bool User::save(rocksdb::DB* db) {
	Json::Value jsonTokens;
	for (UserToken* oneToken : *this->tokens) {
		Json::Value jsonToken;
		jsonToken["token"] = oneToken->token;
		jsonToken["expiration"] = Json::Value::Int64((int64_t) oneToken->expiration);
		jsonTokens.append(jsonToken);
	}
	Json::StyledWriter writer;
	rocksdb::Status status = db->Put(rocksdb::WriteOptions(), "users."+this->email,
									 "{"
											 "\"email\":\""+this->email+"\", "
											 "\"password\":\""+this->hashed_password+"\", "
											 "\"tokens\":" + writer.write(jsonTokens) + "}");
	return (status.ok());
}

void User::signup(rocksdb::DB* db) {
	std::string value;
	if (checkIfExisting(db,&value)) throw AlreadyExistentUserException();
	this->save(db);
}

bool User::checkIfExisting(rocksdb::DB *db, std::string* value) {
	rocksdb::Status status = db->Get(rocksdb::ReadOptions(), "users."+this->email, value);
	if (status.IsNotFound()) {
		return false;
	}

	return true;
}

bool User::checkPassword(std::string password){
	return this->hashPassword(password).compare(this->hashed_password) == 0;
}

/**
 * @throws NonExistentUserException
 * @throws Exception
 */
User* User::load(rocksdb::DB* db, std::string email) {
	std::string value;
	rocksdb::Status status = db->Get(rocksdb::ReadOptions(), "users."+email, &value);

	if (status.IsNotFound()) throw NonExistentUserException();

	Json::Reader reader;
	Json::Value root;
	bool parsingSuccessful = reader.parse(value, root, false);
	if (!parsingSuccessful){ // False for ignoring comments.
		std::cout << "JsonCPP no pudo parsear en User::load. Value: " << value << ". root: " << root << std::endl;
		throw std::exception();
	}

	User* user = new User();
	user->email = root["email"].asString();
	user->hashed_password = root["password"].asString();

	Json::Value tokens = root["tokens"];
	time_t currTime;
	time(&currTime);
	for(Json::ValueIterator it = tokens.begin(); it != tokens.end();it++ ){
		UserToken* userToken = new UserToken();
		userToken->expiration = (*it)["expiration"].asInt64();
		userToken->token = (*it)["token"].asString();
		if (!userToken->hasExpired())
			user->tokens->push_back(userToken);
	}


	return user;
}

void User::setEmail (std::string email) {
	this->email = email;
}

void User::setPassword (std::string password) {
	this->hashed_password = this->hashPassword(password);
}

std::string User::hashPassword (std::string password) {
	unsigned char *out;
	const char* pwd = password.c_str();
	unsigned char salt_value[] = {'s','a','l','t'};

	out = (unsigned char *) malloc(sizeof(unsigned char) * 20);
	char* formatted_out = (char*) malloc(sizeof(unsigned char) * 41);

	PKCS5_PBKDF2_HMAC_SHA1(pwd, strlen(pwd), salt_value, sizeof(salt_value), 1, 20, out);

	for(int i=0;i<20;i++) { sprintf(&(formatted_out[i*2]), "%02x", out[i]); }

	std::string new_password(reinterpret_cast<char*>(formatted_out));
	free(out);
	return new_password;
}

bool User::signin(std::string password) {
	return this->checkPassword(password);
}

/* También limpia tokens vencidos */
bool User::addToken(rocksdb::DB* db, std::string token){
	this->tokens->push_back(new UserToken(token));
	return this->save(db);
}

void User::checkToken(rocksdb::DB* db,std::string token){

	std::string value;
	rocksdb::Status status = db->Get(rocksdb::ReadOptions(), "users."+this->email, &value);
	if (status.IsNotFound()) {
		std::cout << "Entra porque no encontró al user" << std::endl;
		throw NotLoggedInException();
	}

	Json::Reader reader;
	Json::Value root;
	bool parsingSuccessful = reader.parse(value, root, false); // False for ignoring comments.
	if (!parsingSuccessful){
		std::cout << "JsonCPP no pudo parsear en getToken." << std::endl;
		throw UserException();
	}
	Json::Value tokens;

	if(!root.isMember("tokens")) throw RequestException();//User is not initialized properly


	bool hasToken = false;
	tokens = root["tokens"];
	for(Json::ValueIterator it = tokens.begin(); it != tokens.end();it++ ) {
		if((*it)["token"].asString().compare(token) == 0) hasToken = true;
	}

	if(!hasToken) {
		std::cout << "No tiene al token" << std::endl;
		throw NotLoggedInException();
	}

}

