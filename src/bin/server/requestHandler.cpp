#include "requestHandler.h"
#include <cstring>

RequestHandler::RequestHandler() {

	this->userManager = new UserManager();
	this->fileManager = new FileManager();

	this->codesMap = new std::unordered_map<std::string,int>;
	(*this->codesMap)["/users:POST"] = requestCodes::USERS_POST;
	(*this->codesMap)["/users:GET"] = requestCodes::USERS_GET;
	(*this->codesMap)["/login:GET"] = requestCodes::LOGIN_GET;
	(*this->codesMap)["/logout:GET"] = requestCodes::LOGOUT_GET;
	(*this->codesMap)["/files:POST"] = requestCodes::SAVEFILE_POST;
	(*this->codesMap)["/files:GET"] = requestCodes::LOADFILE_GET;
	(*this->codesMap)["/userfiles:GET"] = requestCodes::LOADUSERFILES_GET;
	(*this->codesMap)["/files:DELETE"] = requestCodes::ERASEFILE_DELETE;
	(*this->codesMap)["/filesupload:POST"] = requestCodes::FILEUPLOAD_POST;
	(*this->codesMap)["/shared:POST"] = requestCodes::SHAREFILE_POST;
}

RequestHandler::~RequestHandler(){
	delete this->codesMap;
	delete this->userManager;
	delete this->fileManager;
}

int RequestHandler::handle(std::string uri, std::string request_method, struct mg_connection* conn) {
	// Combine uri+request_method.
	std::string uriPlusMethod = uri + ":" + request_method;
	std::cout << uriPlusMethod << std::endl;
	if (!this->codesMap->count(uriPlusMethod)) {
		return -1;
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
			case requestCodes::USERS_GET: {
				char cemail[100], ctoken[100];
				mg_get_var(conn, "email", cemail, sizeof(cemail));
				mg_get_var(conn, "token", ctoken, sizeof(ctoken));
				if (strlen(cemail) == 0) throw RequestException();
				if (strlen(ctoken) == 0) throw RequestException();

				this->userManager->checkIfLoggedIn(std::string(cemail), std::string(ctoken));
				result = this->userManager->getUsers(std::string(cemail));
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
					throw RequestException();

                int id, size;
                std::string email, token, name, extension, path;

                if (!root.isMember("id")) {
                    id = -1;
				} else {
                    id = root["id"].asInt();
				}
				if (! root.isMember("email") || ! root.isMember("token")) throw RequestException();
				if (! root.isMember("name") || ! root.isMember("extension") ||
					! root.isMember("tags") || !root.isMember("size"))
					throw RequestException();
                if (id == -1 && !root.isMember("path")) throw RequestException();

				email = root["email"].asString();
				token = root["token"].asString();
				name = root["name"].asString();
				extension = root["extension"].asString();
				size = root["size"].asInt(); // Size in MB.
				Json::Value tags = root["tags"];
				std::vector<std::string> vtags;
				for (Json::ValueIterator itr = tags.begin(); itr != tags.end(); itr++) {
					vtags.push_back((*itr).asString());
				}

				this->userManager->checkIfLoggedIn(email, token);
				if (id == -1) {
                    path = root["path"].asString();
					result = this->fileManager->saveFile(email, name, extension, path, vtags, size);
				} else {
                    this->fileManager->checkIfUserHasFilePermits(id, email);
					result = this->fileManager->saveNewVersionOfFile(email, id, name, extension, vtags, size); //TODO terminar bien esta función.
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
                if (strlen(cemail) == 0 || strlen(ctoken) == 0 || strlen(cpath) == 0) throw RequestException();

				this->userManager->checkIfLoggedIn(std::string(cemail), std::string(ctoken));
				result = this->userManager->loadUserFiles(std::string(cemail), std::string(cpath));
				break;
			}
			case requestCodes::SHAREFILE_POST:
			{
				//Needed for filtering unnecesary headers
				char json[conn->content_len + 1];
				char *content = conn->content;
				content[conn->content_len] = '\0';
				strcpy(json, conn->content);
				Json::Value root;
				Json::Reader reader;
				if (!reader.parse(json, root, false))
					throw RequestException();
				
				std::string email, token;
				int id;
				if (! root.isMember("email") || ! root.isMember("token") || ! root.isMember("id")) throw RequestException();
				if (! root.isMember("users") ) throw RequestException();
				
				email = root["email"].asString();
				token = root["token"].asString();
				id = root["id"].asInt();
				std::vector<std::string> users;
				for (Json::ValueIterator itr = root["users"].begin(); itr != root["users"].end(); itr++) {
					users.push_back((*itr).asString());
				}
				
				this->userManager->checkIfLoggedIn(email, token);
				this->fileManager->shareFileToUsers(id, users);
				
			}
			case requestCodes::FILEUPLOAD_POST:
			{
				std::cout << "FILEUPLOAD" << std::endl;
				/*FILE *fp = (FILE *) conn->connection_param;
				if (fp != NULL) {
					fwrite(conn->content, 1, conn->content_len, fp); // Write last bits
					/*mg_printf(conn, "HTTP/1.1 200 OK\r\n"
									  "Content-Type: text/plain\r\n"
									  "Connection: close\r\n\r\n"
									  "Written %ld of POST data to a temp file:\n\n",
							  (long) ftell(fp));*/

					// Temp file will be destroyed after fclose(), do something with the
					// data here -- for example, parse it and extract uploaded files.
				/*
					mg_printf_data(conn, "{ \"result\" : false }");

					FILE* fout = fopen("output.txt", "w");
					rewind(fp);
					int c = fgetc (fp);
					while (c >= 0) {
						fputc (c, fout);
						c = fgetc (fp);
					}
					fclose (fout);
					std::cout << "COPIED" << std::endl;
					fclose(fp);

					// As an example, we just echo the whole POST buffer back to the client.
					//mg_send_file_data(conn, fileno(fp));
					return MG_TRUE;  // Tell Mongoose reply is not completed yet
				} else {
					mg_printf_data(conn, "%s", "Had no data to write...");
					return MG_TRUE; // Tell Mongoose we're done with this request
				}*/

				const char *data;
				int data_len, ofs = 0;
				char var_name[100], file_name[100];

				while ((ofs = mg_parse_multipart(conn->content + ofs, conn->content_len - ofs,
												 var_name, sizeof(var_name),
												 file_name, sizeof(file_name),
												 &data, &data_len)) > 0) {
					FILE* fout = fopen(file_name, "w");
					fwrite(data, data_len, 1, fout);
					fclose(fout);
				}

				mg_printf_data(conn, "{ \"result\" : true }");

				return MG_TRUE;
			}
			default:
				return -1;
		}
		mg_printf_data(conn, result.c_str());

	} catch (std::exception& e) {
		mg_printf_data(conn, "{ \"result\" : false , \"errors\" : [ \"%s\" ] }", e.what()); // Even if there is an error, it should return true to close the connection.
	}
	return -2;
}
