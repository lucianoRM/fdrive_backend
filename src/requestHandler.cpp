#include "requestHandler.h"

RequestHandler::RequestHandler(){

	this->userManager = new UserManager();
	this->fileManager = new FileManager();

	this->codesMap = new std::unordered_map<std::string,int>;

	(*this->codesMap)["/users:GET"] = requestCodes::USERS_POST;
	(*this->codesMap)["/login:GET"] = requestCodes::LOGIN_GET;
	(*this->codesMap)["/logout:GET"] = requestCodes::LOGOUT_GET;
	(*this->codesMap)["/files:POST"] = requestCodes::SAVEFILE_POST;
	(*this->codesMap)["/files:GET"] = requestCodes::LOADFILE_GET;
	(*this->codesMap)["/userfiles:GET"] = requestCodes::LOADUSERFILES_GET;
	(*this->codesMap)["/files:DELETE"] = requestCodes::ERASEFILE_DELETE;

}

RequestHandler::~RequestHandler(){

	delete this->codesMap;
	delete this->userManager;
	delete this->fileManager;

}

bool RequestHandler::handle(std::string uri, std::string request_method, struct mg_connection* conn) {

	//Combine uri+request_method
	std::string uriPlusMethod = uri + ":" + request_method;

	if (!this->codesMap->count(uriPlusMethod)) {
		return false;
	}

	int reqCode = codesMap->at(uriPlusMethod);
	try {
		std::string result;

		switch (reqCode) {
			case requestCodes::USERS_POST: {
				char cemail[100], cpassword[100];
				mg_get_var(conn, "email", cemail, sizeof(cemail));
				mg_get_var(conn, "password", cpassword, sizeof(cpassword));

				result = this->userManager->addUser(std::string(cemail), std::string(cpassword));
				break;
			}
			case requestCodes::LOGIN_GET: {
				char cemail[100], cpassword[100];
				mg_get_var(conn, "email", cemail, sizeof(cemail));
				mg_get_var(conn, "password", cpassword, sizeof(cpassword));

				result = this->userManager->loginUser(std::string(cemail), std::string(cpassword));
				break;
			}
			case requestCodes::LOGOUT_GET: {
				char cemail[100], ctoken[100];
				mg_get_var(conn, "email", cemail, sizeof(cemail));
				mg_get_var(conn, "token", ctoken, sizeof(ctoken));

				result = this->userManager->logoutUser(std::string(cemail), std::string(ctoken));
				break;
			}
			case requestCodes::SAVEFILE_POST:
			{
				//Needed for filtering unnecesary headers
				char json[conn->content_len + 1];
				char *content = conn->content;
				content[conn->content_len] = '\0';
				strcpy(json, conn->content);
				Json::Value root;
				Json::Reader reader;
				if (!reader.parse(json, root, false))
					throw FileException();

                int id;
                std::string email, token, name, extension, owner;

                if (root["id"].isNull()) {
                    id = -1;
				} else {
                    id = root["id"].asInt();
				}
				if (!root.isMember("email") || !root.isMember("token")) throw RequestException();
				if (!root.isMember("name") || !root.isMember("extension") ||
					!root.isMember("tags"))
					throw RequestException();

				email = root["email"].asString();
				token = root["token"].asString();

				name = root["name"].asString();
				extension = root["extension"].asString();
				Json::Value tags = root["tags"];
				std::vector<std::string> vtags;
				for (Json::ValueIterator itr = tags.begin(); itr != tags.end(); itr++) {
					vtags.push_back((*itr).asString());
				}

				this->userManager->checkIfLoggedIn(email, token);
				if (id == -1) {
					result = this->fileManager->saveFile(email, name, extension, vtags);
				} else {
					result = this->fileManager->saveNewVersionOfFile(email, id, name, extension, vtags);
				}
				break;
			}
			case requestCodes::LOADFILE_GET: {
				char cemail[100], ctoken[100], id[100];
				mg_get_var(conn, "email", cemail, sizeof(cemail));
				mg_get_var(conn, "token", ctoken, sizeof(ctoken));
				mg_get_var(conn, "id", id, sizeof(id));

				this->userManager->checkIfLoggedIn(std::string(cemail), std::string(ctoken));
				result = this->fileManager->loadFile(atoi(id));
				break;
			}
			case requestCodes::ERASEFILE_DELETE:
			{
				//Needed for filtering unnecesary headers
				char json[conn->content_len + 1];
				char *content = conn->content;
				content[conn->content_len] = '\0';
				strcpy(json, conn->content);
				Json::Value root;
				Json::Reader reader;
				if (!reader.parse(json, root, false))
					throw FileException();
				if (!root.isMember("email") || !root.isMember("token")) throw RequestException();
				if (!root.isMember("name") || !root.isMember("extension") || !root.isMember("owner") ||
					!root.isMember("tags"))
					throw RequestException();
				if (!root.isMember("id")) throw RequestException();
				std::string email, token;
				int id;
				email = root["email"].asString();
				token = root["ctoken"].asString();
				id = root["id"].asInt();

				this->userManager->checkIfLoggedIn(email, token);
				result = this->fileManager->eraseFile(id);
				break;
			}
			case requestCodes::LOADUSERFILES_GET:
			{
				char cemail[100], ctoken[100];
				mg_get_var(conn, "email", cemail, sizeof(cemail));
				mg_get_var(conn, "token", ctoken, sizeof(ctoken));

				this->userManager->checkIfLoggedIn(std::string(cemail), std::string(ctoken));
				result = this->userManager->loadUserFiles(std::string(cemail));
				break;
			}
			default:
				return false;
		}
		mg_printf_data(conn, result.c_str());

	} catch (std::exception& e) {
		mg_printf_data(conn, "{ \"result\" : false , \"errors\" : [ \"%s\" ] }", e.what()); //Even if there is an error, it should return true to close the connection
	}
	return true;
}
