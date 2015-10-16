#ifndef REQHANDLER_H_
#define REQHANDLER_H_

#include <string.h>
#include <unordered_map>
#include "mongoose.h"
#include "userManager.h"
#include "fileManager.h"
#include <vector>



//Needed for switch case.
enum requestCodes : int { USERS_POST = 1,LOGIN_GET,SAVEFILE_POST,LOADFILE_GET,ERASEFILE_DELETE, LOADUSERFILES_GET };


class RequestHandler {



	private:

		UserManager* userManager;
		FileManager* fileManager;

		std::unordered_map<std::string,int >* codesMap;


	public:
	
		RequestHandler();

		~RequestHandler();

		// Devuelve true si era una request válida (más allá de su resultado)
		// o false sino.
		bool handle(std::string uri, std::string request_method, struct mg_connection* conn);

};


#endif /* REQHANDLER_H_ */
