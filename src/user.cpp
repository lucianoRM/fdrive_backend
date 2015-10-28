#include "user.h"

User::User(){
	this->tokens = new std::vector<UserToken*>();
	this->files = new std::vector<struct userFile*>();
}

User::~User(){
	for (UserToken* token : *this->tokens) {
		delete token;
	}
	delete this->tokens;
	for (struct userFile* file: *this->files) {
		delete file;
	}
	delete this->files;
}

std::string User::getEmail() {
	return this->email;
}

void User::deleteExpiredTokens(time_t* currTime) {
	std::vector<UserToken*>::iterator it = this->tokens->begin();
	while (it != this->tokens->end()) {
		if ((*it)->hasExpired(currTime)) {
			it = this->tokens->erase(it);
		} else {
			it++;
		}
	}
}


bool User::save(rocksdb::DB* db) {
	Json::Value jsonTokens;
	for (UserToken* oneToken : *this->tokens) {
		jsonTokens.append(oneToken->serialize());
	}

	Json::Value jsonFiles;
	for (struct userFile* file: *this->files) {
		Json::Value jsonFile;
		jsonFile["id"] = file->id;
		jsonFile["permits"] = file->permits;
		jsonFiles.append(jsonFile);
	}

	Json::StyledWriter writer;
	rocksdb::Status status = db->Put(rocksdb::WriteOptions(), "users."+this->email,
									 "{"
											 "\"email\":\""+this->email + "\", "
											 "\"password\":\""+this->hashed_password + "\", "
											 "\"tokens\":" + writer.write(jsonTokens) + ", "
									         "\"files\":" + writer.write(jsonFiles) + "}");
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
 * Elimina tokens expirados.
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
	for(Json::ValueIterator it = tokens.begin(); it != tokens.end();it++ ){
		UserToken* userToken = UserToken::deserialize((*it));
		if (!userToken->hasExpired())
			user->tokens->push_back(userToken);
	}

	Json::Value files = root["files"];
	for(Json::ValueIterator it = files.begin(); it != files.end();it++ ){
		struct userFile* file = new struct userFile;
		file->id = (*it)["id"].asInt();
		file->permits = (*it)["permits"].asString();
		user->files->push_back(file);
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

bool User::login(std::string password) {
	return this->checkPassword(password);
}

bool User::logout(rocksdb::DB* db, std::string token) {
	for (std::vector<UserToken*>::iterator it = this->tokens->begin() ; it != this->tokens->end(); ++it) {
		if ( token.compare((*it)->token) == 0 && !(*it)->hasExpired()) {
			this->tokens->erase(it);
			return this->save(db);
		}
	}
	return false;
}

std::string User::getNewToken(rocksdb::DB* db) {
	UserToken* userToken = UserToken::getRandomToken();
	this->tokens->push_back(userToken);
	this->save(db);
	return userToken->token;
}

void User::checkToken(std::string token) {

	this->deleteExpiredTokens();

	for (std::vector<UserToken*>::iterator it = this->tokens->begin(); it != this->tokens->end(); it++) {
		if ((*it)->token.compare(token) == 0) return;
	}

	throw NotLoggedInException();

}


void User::addFile(int id) {
	struct userFile* file = new struct userFile;
	file->id = id;
	file->permits = "O"; // O de owner.
	this->files->push_back(file);
}

void User::addSharedFile(int id) {
	struct userFile* file = new struct userFile;
	file->id = id;
	file->permits = "S"; // S de shared.
	this->files->push_back(file);
}

std::vector<int> User::getFiles() {
	std::vector<int> filesID;
	for (struct userFile* file: *this->files) {
		filesID.push_back(file->id);
	}
	return filesID;
}