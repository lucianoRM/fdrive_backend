#include "requestHandler.h"
#include <cstring>

RequestHandler::RequestHandler() {

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
	// Combine uri+request_method.
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
				if (strlen(cemail) == 0) throw RequestException();
				if (strlen(cpassword) == 0) throw RequestException();

				result = this->userManager->addUser(std::string(cemail), std::string(cpassword));
				break;
			}
			case requestCodes::LOGIN_GET: {
				char cemail[100], cpassword[100];
				mg_get_var(conn, "email", cemail, sizeof(cemail));
				mg_get_var(conn, "password", cpassword, sizeof(cpassword));
                if (strlen(cemail) == 0) throw RequestException();
                if (strlen(cpassword) == 0) throw RequestException();

				result = this->userManager->loginUser(std::string(cemail), std::string(cpassword));
				break;
			}
			case requestCodes::LOGOUT_GET: {
				char cemail[100], ctoken[100];
				mg_get_var(conn, "email", cemail, sizeof(cemail));
				mg_get_var(conn, "token", ctoken, sizeof(ctoken));
                if (strlen(cemail) == 0) throw RequestException();
                if (strlen(ctoken) == 0) throw RequestException();

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
                std::string email, token, name, extension, path;

                if (!root.isMember("id")) {
                    id = -1;
				} else {
                    id = root["id"].asInt();
				}
				if (! root.isMember("email") || ! root.isMember("token")) throw RequestException();
				if (! root.isMember("name") || ! root.isMember("extension") ||
					! root.isMember("tags") )
					throw RequestException();
                if (id == -1 && !root.isMember("path")) throw RequestException();

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
                    path = root["path"].asString();
					result = this->fileManager->saveFile(email, name, extension, path, vtags);
				} else {
                    this->fileManager->checkIfUserHasFilePermits(id, email);
					result = this->fileManager->saveNewVersionOfFile(email, id, name, extension, vtags); //TODO terminar bien esta función.
				}
				break;
			}
			case requestCodes::LOADFILE_GET: {
				char cemail[100], ctoken[100], cid[100];
				mg_get_var(conn, "email", cemail, sizeof(cemail));
				mg_get_var(conn, "token", ctoken, sizeof(ctoken));
				mg_get_var(conn, "id", cid, sizeof(cid));
                if (strlen(cemail) == 0) throw RequestException();
                if (strlen(ctoken) == 0) throw RequestException();
                if (strlen(cid) == 0) throw RequestException();

				this->userManager->checkIfLoggedIn(std::string(cemail), std::string(ctoken));
				this->fileManager->checkIfUserHasFilePermits(atoi(cid), std::string(cemail));
				result = this->fileManager->loadFile(atoi(cid));
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
				if (!root.isMember("id") || !root.isMember("path")) throw RequestException();

				std::string email, token, path;
				int id;
				email = root["email"].asString();
				token = root["ctoken"].asString();
				id = root["id"].asInt();
				path = root["path"].asString();

				this->userManager->checkIfLoggedIn(email, token);
                this->fileManager->checkIfUserHasFilePermits(id, email);
				result = this->fileManager->eraseFileFromUser(id, email, path);
				break;
			}
			case requestCodes::LOADUSERFILES_GET:
			{
				char cemail[100], ctoken[100], cpath[100];
				mg_get_var(conn, "email", cemail, sizeof(cemail));
				mg_get_var(conn, "token", ctoken, sizeof(ctoken));
				mg_get_var(conn, "path", cpath, sizeof(cpath));
                if (strlen(cemail) == 0) throw RequestException();
                if (strlen(ctoken) == 0) throw RequestException();
                if (strlen(cpath) == 0) throw RequestException();

				this->userManager->checkIfLoggedIn(std::string(cemail), std::string(ctoken));
				result = this->userManager->loadUserFiles(std::string(cemail), std::string(cpath));
				break;
			}
			default:
				return false;
		}
		mg_printf_data(conn, result.c_str());

	} catch (std::exception& e) {
		mg_printf_data(conn, "{ \"result\" : false , \"errors\" : [ \"%s\" ] }", e.what()); // Even if there is an error, it should return true to close the connection.
	}
	return true;
}
