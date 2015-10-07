//
// Created by agustin on 10/09/15.
//

#include "user.h"


User::User(std::string email) {
	this->email = email;
}

std::string User::getEmail() {
	return this->email;
}

bool User::save(rocksdb::DB* db) {
	rocksdb::Status status = db->Put(rocksdb::WriteOptions(), "users."+this->email, "{\"email\":\""+this->email+"\", \"password\":\""+this->hashed_password+"\"}");
	return (status.ok());
}

bool User::signup(rocksdb::DB* db,std::string password) {
	std::string value;
	if (checkIfExisting(db,&value)) {
		return false;
	}

	this->hashed_password = this->hashPassword(password);
    return this->save(db);
}

bool User::checkIfExisting(rocksdb::DB *db, std::string* value) {
	rocksdb::Status status = db->Get(rocksdb::ReadOptions(), "users."+this->email, value);
	if (status.IsNotFound()) {
		return false;
	}

	return true;
}

bool User::checkPassword(rocksdb::DB* db, std::string password){
	std::string value;

	if (! checkIfExisting(db,&value)) {
		return false;
	}

	Json::Reader reader;
	Json::Value root;
	bool parsingSuccessful = reader.parse(value, root, false);
	if (!parsingSuccessful){ // False for ignoring comments.
		std::cout << "JsonCPP no pudo parsear en User::load. Value: " << value << ". root: " << root << std::endl;
		return false;
	}

	this->hashed_password = root.get("password", "").asString();

	std::string passwordHashed = this->hashPassword(password);

	if (passwordHashed.compare(this->hashed_password) == 0) {
		return true;
	}
	return false;
}

bool User::load(rocksdb::DB* db, std::string password) {

	return checkPassword(db,password);
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
	delete out;
	return new_password;
}

/* También limpia tokens vencidos */
bool User::addToken(rocksdb::DB* db, std::string token){


	std::string value;
    rocksdb::Status status = db->Get(rocksdb::ReadOptions(), "users."+this->email, &value);
    if (status.IsNotFound()) return false;
    
    Json::Reader reader;
    Json::Value root;
    bool parsingSuccessful = reader.parse(value, root, false); // False for ignoring comments.
    if (!parsingSuccessful){
		std::cout << "JsonCPP no pudo parsear en addToken." << std::endl;
		return false;
	}
	Json::Value tokens;
	Json::Value newTokens;
	/*time_t currTime;
	time(&currTime);
	time_t tokenTime;
	//If tokens exists get them and remove expired ones.
	if (root.isMember("tokens")){
		tokens = root["tokens"];
		for(Json::Value::iterator it = tokens.begin(); it != tokens.end();it++ ){
			tokenTime = (*it)["expiration"].asInt64();
			if(difftime(currTime,tokenTime) < 0) { //Not expired yet
				newTokens.append(*it);//Add it again to the tokens
			}
		}

	}*/
	Json::Value jsonToken;
	jsonToken["token"] = token;
	/*int64_t expiration = (int64_t)currTime + 1800;
	jsonToken["expiration"] = std::string(expiration); // Lasts half an hour*/
	newTokens.append(jsonToken);
	root["tokens"] = newTokens;
	
	Json::StyledWriter writer;
	status = db->Put(rocksdb::WriteOptions(), "users."+this->email, writer.write(root));
	if (!status.ok()){
		std::cout << "Error al guardar token nuevo en usuario: " << email << "." << std::endl;
		return false;
	}
    return true;
	
}

void User::checkToken(rocksdb::DB* db,std::string token){

	std::string value;
	rocksdb::Status status = db->Get(rocksdb::ReadOptions(), "users."+this->email, &value);
	if (status.IsNotFound()) {
		delete db;
		throw errorCode::NOT_LOGGED_IN;
	}


	Json::Reader reader;
	Json::Value root;
	bool parsingSuccessful = reader.parse(value, root, false); // False for ignoring comments.
	if (!parsingSuccessful){
		std::cout << "JsonCPP no pudo parsear en getToken." << std::endl;
		delete db;
		throw -1;
	}
	Json::Value tokens;

	if(!root.isMember("tokens")) {
		delete db;
		throw -1;
	} //User is not initialized properly


	bool hasToken = false;
	//Looks for the token
	tokens = root["tokens"];
	for(Json::Value::iterator it = tokens.begin(); it != tokens.end();it++ ) {
		if((*it)["token"].asString() == token) hasToken = true;
	}

	if(!hasToken) {
		delete db;

		throw errorCode::NOT_LOGGED_IN;
	}


}

