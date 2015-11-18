#ifndef REQHANDLER_H_
#define REQHANDLER_H_

#include <string.h>
#include <unordered_map>
#include "mongoose.h"
#include "userManager.h"
#include "fileManager.h"
#include "folderManager.h"
#include "routeTree.h"
#include <vector>
#include <regex>

// Needed for switch case.
enum requestCodes : int { USERS_POST = 1, USERS_GET, USERS_PUT,
						LOGIN_GET, LOGOUT_GET,
						SAVEFILE_POST, SAVEFILE_PUT,
						LOADFILE_GET, ERASEFILE_DELETE, RECOVERFILE_GET,
						FILEUPLOAD_POST, FILEDOWNLOAD_GET,
						LOADUSERFILES_GET,
						SHAREFILE_POST, SHAREFOLDER_POST, SHAREFILE_DELETE,
						ADDFOLDER_POST, RENAMEFOLDER_PUT,
						SEARCHES_GET, CLEAN_DB };

// Class to manage the requests done by the user.

class RequestHandler {
	private:
		UserManager* userManager;
		FileManager* fileManager;
		FolderManager* folderManager;

		RouteTree* routeTree;
		rocksdb::DB* database;



	public:
		RequestHandler(rocksdb::DB* database, bool testing);
		~RequestHandler();

		// Returns true if was a valid request (no matter its result).
		// False in other case.
		int handle(std::string uri, std::string request_method, struct mg_connection* conn);
};

#endif /* REQHANDLER_H_ */
