//
// Created by agustin on 10/09/15.
//

#include "user.h"
#include "UserException.h"


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

void User::signup(rocksdb::DB* db,std::string password) {
	std::string value;
	if (checkIfExisting(db,&value)) throw AlreadyExistentUserException();


	this->hashed_password = this->hashPassword(password);
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
void User::load(rocksdb::DB* db) {
	std::string value;
	rocksdb::Status status = db->Get(rocksdb::ReadOptions(), "users."+this->email, &value);

	if (status.IsNotFound()) throw NonExistentUserException();

	Json::Reader reader;
	Json::Value root;
	bool parsingSuccessful = reader.parse(value, root, false);
	if (!parsingSuccessful){ // False for ignoring comments.
		std::cout << "JsonCPP no pudo parsear en User::load. Value: " << value << ". root: " << root << std::endl;
		throw std::exception();
	}

	this->email = root["email"].asString();
	this->hashed_password = root["password"].asString();

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

bool User::signin(std::string password) {
	return this->checkPassword(password);
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
	if (status.IsNotFound()) throw NotLoggedInException();



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
	//Looks for the token
	tokens = root["tokens"];
	for(Json::Value::iterator it = tokens.begin(); it != tokens.end();it++ ) {
		if((*it)["token"].asString() == token) hasToken = true;
	}

	if(!hasToken) throw NotLoggedInException();



}

