#include "requestHandler.h"
#include <cstring>
#include <sys/stat.h>
#include <server/server.h>
#include "fileSystemExceptions.h"


RequestHandler::RequestHandler(rocksdb::DB* database, bool testing) {
	this->database = database;

	this->userManager = new UserManager(database);
	this->fileManager = new FileManager(database);
	this->folderManager = new FolderManager(database);

	this->routeTree = new RouteTree();

	if (testing) {
		this->routeTree->add("cleandb", "POST", requestCodes::CLEAN_DB);
	}

	this->routeTree->add("users", "POST", requestCodes::USERS_POST);
	this->routeTree->add("users", "GET", requestCodes::USERS_GET);
	this->routeTree->add("users", "PUT", requestCodes::USERS_PUT);

	this->routeTree->add("login", "GET", requestCodes::LOGIN_GET);

	this->routeTree->add("logout", "GET", requestCodes::LOGOUT_GET);

	this->routeTree->add("files/:int", "DELETE", requestCodes::ERASEFILE_DELETE);

	this->routeTree->add("files/:int/metadata", "GET", requestCodes::LOADFILE_GET);
	this->routeTree->add("files/:int/:int/metadata", "GET", requestCodes::LOADFILE_GET);
	this->routeTree->add("files/:int/metadata", "POST", requestCodes::SAVEFILE_POST);
	this->routeTree->add("files/metadata", "POST", requestCodes::SAVEFILE_POST);
	this->routeTree->add("files/:int/metadata", "PUT", requestCodes::SAVEFILE_PUT);

	this->routeTree->add("files/:int/:int/data", "POST", requestCodes::FILEUPLOAD_POST);
	this->routeTree->add("files/:int/:int/data", "GET", requestCodes::FILEDOWNLOAD_GET);
	this->routeTree->add("files/:int/data", "GET", requestCodes::FILEDOWNLOAD_GET);

	this->routeTree->add("userfiles", "GET", requestCodes::LOADUSERFILES_GET);

	this->routeTree->add("share", "POST", requestCodes::SHAREFILE_POST);
	this->routeTree->add("unshare", "PUT", requestCodes::SHAREFILE_DELETE);
	this->routeTree->add("share/folder", "POST", requestCodes::SHAREFOLDER_POST);

	this->routeTree->add("folders", "POST", requestCodes::ADDFOLDER_POST);

	this->routeTree->add("folders", "PUT", requestCodes::RENAMEFOLDER_PUT);

	this->routeTree->add("searches", "GET", requestCodes::SEARCHES_GET);

	this->routeTree->add("recoverfile", "GET", requestCodes::RECOVERFILE_GET);

}

RequestHandler::~RequestHandler(){
	delete this->userManager;
	delete this->fileManager;
	delete this->folderManager;
}

int RequestHandler::handle(std::string uri, std::string request_method, struct mg_connection* conn) {
	// Combine uri+request_method.
	std::string uriPlusMethod = uri + ":" + request_method;
	//std::cout << uriPlusMethod << std::endl;

	std::string s = uri.substr(1);
	int reqCode;
	try {
		reqCode = this->routeTree->get(s, request_method);
	} catch (RouteNotFoundException e) {
		std::cout << uriPlusMethod << " NOT FOUND" << std::endl;
		return -1;
	}
	//std::cout << reqCode << " METADATA " << requestCodes::LOADFILE_GET << std::endl;


	std::vector<std::string>* routeParameterVector = routeTree->getRouteParameterVector(s);

	try {
		std::string result;

		switch (reqCode) {
			case requestCodes::CLEAN_DB: {
				rocksdb::DB** databasePtrPtr = &(this->database);
				delete this->database;
				system("rm -rf testdb");
				system("rm -rf files");

				rocksdb::Options options;
				options.create_if_missing = true;
				rocksdb::Status status;

                mkdir("files", S_IRWXU | S_IRWXG | S_IRWXO);
				status = rocksdb::DB::Open(options, "testdb", databasePtrPtr);
				Server::database = *databasePtrPtr;

				system("chmod -R a+rwx testdb");

				result = "{ \"result\" : true }";
				break;
			}
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
            case requestCodes::USERS_PUT: {
                //Needed for filtering unnecesary headers
                char json[conn->content_len + 1];
                char *content = conn->content;
                content[conn->content_len] = '\0';
                strcpy(json, conn->content);
                Json::Value root;
                Json::Reader reader;
                if (!reader.parse(json, root, false))
                    throw RequestException();

                std::string email, token, name, lastLocation;

                if (!root.isMember("email") || !root.isMember("token") ) throw RequestException();
                if (!root.isMember("name") && !root.isMember("lastLocation") ) throw RequestException();
                // La foto de perfil se maneja del cliente.
                email = root["email"].asString();
                token = root["token"].asString();
                if (root.isMember("name")) name = root["name"].asString();
                if (root.isMember("lastLocation")) name = root["lastLocation"].asString();

                this->userManager->checkIfLoggedIn(email, token);
                result = this->userManager->saveUserData(email, name, lastLocation);
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

				this->userManager->checkIfLoggedIn(std::string(cemail), std::string(ctoken));
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

                int id, size, version;
                std::string email, token, name, extension, path;
				bool overwrite;

				if (routeParameterVector->size() == 2) {
					id = -1;
				} else {
					id = atoi(routeParameterVector->at(1).c_str());
				}
				if (! root.isMember("email") || ! root.isMember("token")) throw RequestException();
				if (! root.isMember("name") || ! root.isMember("extension")) throw RequestException();
				if (! root.isMember("tags") || !root.isMember("size")) throw RequestException();
                if (id == -1 && !root.isMember("path")) throw RequestException();
				if (id != -1 && !root.isMember("version") && !root.isMember("overwrite")) throw RequestException();

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
					version = root["version"].asInt();
					overwrite = root["overwrite"].asBool();
                    this->fileManager->checkIfUserHasFilePermits(id, email);
					result = this->fileManager->saveNewVersionOfFile(email, id, version, overwrite, name, extension, vtags, size);
				}
				break;
			}
			case requestCodes::SAVEFILE_PUT: {
				//Needed for filtering unnecesary headers
				char json[conn->content_len + 1];
				char *content = conn->content;
				content[conn->content_len] = '\0';
				strcpy(json, conn->content);
				Json::Value root;
				Json::Reader reader;
				if (!reader.parse(json, root, false))
					throw RequestException();

				std::string email, token, name, tag;
                int id;
				if (! root.isMember("email") || ! root.isMember("token")) throw RequestException();
				if (! root.isMember("name") && ! root.isMember("tag")) throw RequestException();

				id = atoi(routeParameterVector->at(1).c_str());
                email = root["email"].asString();
                token = root["token"].asString();
                if (root.isMember("name")) name = root["name"].asString();
                if (root.isMember("tag")) tag = root["tag"].asString();

                this->userManager->checkIfLoggedIn(email, token);
                this->fileManager->checkIfUserHasFilePermits(id, email);
                result = this->fileManager->changeFileData(id, name, tag);
                break;
			}
			case requestCodes::LOADFILE_GET: {
				char cemail[100], ctoken[100];
				mg_get_var(conn, "email", cemail, sizeof(cemail));
				mg_get_var(conn, "token", ctoken, sizeof(ctoken));
				if (strlen(cemail) == 0) throw RequestException();
                if (strlen(ctoken) == 0) throw RequestException();

				int id = atoi(routeParameterVector->at(1).c_str());

                this->userManager->checkIfLoggedIn(std::string(cemail), std::string(ctoken));
                this->fileManager->checkIfUserHasFilePermits(id, std::string(cemail));

                if (routeParameterVector->size() == 4) {
					int version = atoi(routeParameterVector->at(2).c_str());
                    result = this->fileManager->loadFile(id, version);
                } else {
                    result = this->fileManager->loadFile(id);
                }
				break;
			}
			case requestCodes::ERASEFILE_DELETE:
			{

				char email[100], token[100],path[300];
				mg_get_var(conn, "email", email, sizeof(email));
				mg_get_var(conn, "token", token, sizeof(token));
				mg_get_var(conn, "path", path, sizeof(path));
				if (strlen(email) == 0) throw RequestException();
				if (strlen(token) == 0) throw RequestException();
				if (strlen(path) == 0) throw RequestException();

				int id = atoi(routeParameterVector->at(1).c_str());


				this->userManager->checkIfLoggedIn(std::string(email), std::string(token));
                this->fileManager->checkIfUserHasFilePermits(id, std::string(email));
				result = this->fileManager->eraseFileFromUser(id, std::string(email), std::string(path));
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
				this->fileManager->checkIfUserIsOwner(id, email);
				result = this->fileManager->shareFileToUsers(id, users);
				break;
			}
            case requestCodes::SHAREFILE_DELETE:
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
                email = root["email"].asString();
                token = root["token"].asString();
                id = root["id"].asInt();
                std::vector<std::string> users;
                if (root.isMember("users")) {
                    for (Json::ValueIterator itr = root["users"].begin(); itr != root["users"].end(); itr++) {
                        users.push_back((*itr).asString());
                    }
                }

                this->userManager->checkIfLoggedIn(email, token);
                this->fileManager->checkIfUserIsOwner(id, email);
                result = this->fileManager->deleteFileSharedPermits(id, users);
                break;
            }
            case requestCodes::SHAREFOLDER_POST:
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

                std::string email, token, path;
                if (! root.isMember("email") || ! root.isMember("token") || ! root.isMember("path")) throw RequestException();
                if (! root.isMember("users") ) throw RequestException();

                email = root["email"].asString();
                token = root["token"].asString();
                path = root["path"].asString();
                std::vector<std::string> users;
                for (Json::ValueIterator itr = root["users"].begin(); itr != root["users"].end(); itr++) {
                    users.push_back((*itr).asString());
                }


                this->userManager->checkIfLoggedIn(email, token);
                result = this->fileManager->shareFolder(email, path, users);
                break;
            }
			case requestCodes::FILEUPLOAD_POST:
			{
				//std::cout << "FILEUPLOAD" << std::endl;
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

				std::string email, token;
				int id;
				char cemail[100], ctoken[100], cid[100];
				mg_get_var(conn, "email", cemail, sizeof(cemail));
				email = std::string(cemail);
				mg_get_var(conn, "token", ctoken, sizeof(ctoken));
				token = std::string(ctoken);
				id = atoi(routeParameterVector->at(1).c_str());
				int version = atoi(routeParameterVector->at(2).c_str());

				const char *data;
				char *filedata;
				int filedata_len, data_len, ofs = 0;
				char var_name[400], file_name[400];
				while ((ofs = mg_parse_multipart(conn->content + ofs, conn->content_len - ofs,
												 var_name, sizeof(var_name),
												 file_name, sizeof(file_name),
												 &data, &data_len)) > 0) {
					if (std::string(var_name) == "upload") {
						filedata = (char*) malloc(data_len+1);
						memcpy(filedata, data, data_len);
						filedata_len = data_len;
					}
				}
				this->userManager->checkIfLoggedIn(email, token);
				File* file = this->fileManager->openFile(id);
				this->fileManager->checkIfUserIsOwner(file->getId(), email);
				FILE* fout = fopen(("files/"+file->getOwner()+"/"+file->getMetadata()->ownerPath+"/"+std::to_string(id)+"."+std::to_string(version)).c_str(), "w");
				if (fout == NULL) throw FileSystemException();
				fwrite(filedata, filedata_len, 1, fout);
				free(filedata);
				fclose(fout);
				mg_printf_data(conn, "{ \"result\" : true }");

				return MG_TRUE;

				break;
			}
			case requestCodes::FILEDOWNLOAD_GET:
			{
				std::string email, token;
				int id;
				char cemail[100], ctoken[100], cid[100];

				mg_get_var(conn, "email", cemail, sizeof(cemail));
				email = std::string(cemail);
				mg_get_var(conn, "token", ctoken, sizeof(ctoken));
				token = std::string(ctoken);
				id = atoi(routeParameterVector->at(1).c_str());
				int version;
				if (routeParameterVector->size() == 4)
					version = atoi(routeParameterVector->at(2).c_str());
				else
					version = -1;
				this->userManager->checkIfLoggedIn(email, token);
				File* file = this->fileManager->openFile(id);
				this->fileManager->checkIfUserIsOwner(file->getId(), email);
				if (version == -1)
					version = file->getLatestVersion();
				const char* path = ("files/"+file->getOwner()+"/"+file->getMetadata()->ownerPath+"/"+std::to_string(id)+"."+std::to_string(version)).c_str();
				const char* extraHeaders = ("Content-Disposition: attachment; filename=\""+file->getMetadata()->name+file->getMetadata()->extension+"\"\r\n").c_str();
				mg_send_file(conn, path, extraHeaders);
				return MG_MORE;

				break;
			}

			case requestCodes::ADDFOLDER_POST:
			{
				char cemail[100], ctoken[100], cpath[100], cname[100];
				mg_get_var(conn, "email", cemail, sizeof(cemail));
				mg_get_var(conn, "token", ctoken, sizeof(ctoken));
				mg_get_var(conn, "path", cpath, sizeof(cpath));
				mg_get_var(conn, "name", cname, sizeof(cname));
				if (strlen(cemail) == 0 || strlen(ctoken) == 0 || strlen(cpath) == 0 || strlen(cname) == 0) throw RequestException();

				this->userManager->checkIfLoggedIn(cemail, ctoken);
				result = this->folderManager->addFolder(std::string(cemail), std::string(cpath), std::string(cname));
				break;
			}

			case requestCodes::RENAMEFOLDER_PUT:
			{
				char cemail[100], ctoken[100], cpath[100], cnameold[100], cnamenew[100];
				mg_get_var(conn, "email", cemail, sizeof(cemail));
				mg_get_var(conn, "token", ctoken, sizeof(ctoken));
				mg_get_var(conn, "path", cpath, sizeof(cpath));
				mg_get_var(conn, "nameold", cnameold, sizeof(cnameold));
				mg_get_var(conn, "namenew", cnamenew, sizeof(cnamenew));
				if (strlen(cemail) == 0 || strlen(ctoken) == 0 || strlen(cpath) == 0 || strlen(cnameold) == 0 || strlen(cnamenew) == 0) throw RequestException();

				this->userManager->checkIfLoggedIn(cemail, ctoken);
				result = this->folderManager->renameFolder(cemail,cpath,cnameold,cnamenew);
				break;
			}
			case requestCodes::SEARCHES_GET:
			{
				char cemail[100], ctoken[100], cpath[100], ctypeOfSearch[100], celement[100];
				mg_get_var(conn, "email", cemail, sizeof(cemail));
				mg_get_var(conn, "token", ctoken, sizeof(ctoken));
				mg_get_var(conn, "typeofsearch", ctypeOfSearch, sizeof(ctypeOfSearch));
				mg_get_var(conn, "element", celement, sizeof(celement));
				if (strlen(cemail) == 0 || strlen(ctoken) == 0 || strlen(cpath) == 0 || strlen(ctypeOfSearch) == 0 || strlen(celement) == 0) throw RequestException();

				this->userManager->checkIfLoggedIn(std::string(cemail), std::string(ctoken));
				result = this->fileManager->getSearches(std::string(cemail),std::string(ctypeOfSearch),std::string(celement));
				break;
			}
			case requestCodes::RECOVERFILE_GET:
			{
				char email[100], token[100], id[100];
				mg_get_var(conn, "email", email, sizeof(email));
				mg_get_var(conn, "token", token, sizeof(token));
				mg_get_var(conn, "id", id, sizeof(id));
				if (strlen(email) == 0) throw RequestException();
				if (strlen(token) == 0) throw RequestException();
				if (strlen(id) == 0) throw RequestException();

				this->userManager->checkIfLoggedIn(std::string(email), std::string(token));
				result = this->fileManager->recoverFile(std::string(email), atoi(id));
				break;
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
