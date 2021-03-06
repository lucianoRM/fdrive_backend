#ifndef REQHANDLER_H_
#define REQHANDLER_H_

#include <string.h>
#include <unordered_map>
#include "mongoose.h"
#include "userManager.h"
#include "fileManager.h"
#include <vector>

// Needed for switch case.
enum requestCodes : int { USERS_POST = 1, LOGIN_GET, LOGOUT_GET,
		SAVEFILE_POST, LOADFILE_GET, ERASEFILE_DELETE, LOADUSERFILES_GET,
		FILEUPLOAD_POST, USERS_GET };

// Class to manage the requests done by the user.

class RequestHandler {
	private:
		UserManager* userManager;
		FileManager* fileManager;

		std::unordered_map<std::string,int >* codesMap;

	public:
		RequestHandler();
		~RequestHandler();

		// Returns true if was a valid request (no matter its result).
		// False in other case.
		int handle(std::string uri, std::string request_method, struct mg_connection* conn);
};

#endif /* REQHANDLER_H_ */
