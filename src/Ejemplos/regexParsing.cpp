#include <string>
#include <regex>
#include <iostream>


void prueba1() {
	std::string toParse = "/users/id_user/files:GET";
	std::cout << "En string: " << toParse << std::endl;
	size_t index = toParse.find("/users/");
	std::cout << "/users/ en " << index << std::endl;
	index = toParse.find("/files:");
	std::cout << "/files: en " << index << std::endl;
	
	toParse = "/users/id_user/files/id_file:GET";
	std::cout << "En string: " << toParse << std::endl;
	index = toParse.find("/users/");
	if (index == std::string::npos) {
		std::cout << "No encontró /users/" << std::endl;
	} else {
		std::cout << "/users/ en " << index << std::endl;
	}
	index = toParse.find("/files:");
	if (index == std::string::npos) {
		std::cout << "No encontró /files:" << std::endl;
	} else {
		std::cout << "/files: en " << index << std::endl;
	}
	toParse = "/login:GET";
	std::cout << "En string: " << toParse << std::endl;
	index = toParse.find("/users/");
	if (index == std::string::npos) {
		std::cout << "No encontró /users/" << std::endl;
	} else {
		std::cout << "/users/ en " << index << std::endl;
	}
	index = toParse.find("/files:");
	if (index == std::string::npos) {
		std::cout << "No encontró /files:" << std::endl;
	} else {
		std::cout << "/files: en " << index << std::endl;
	}
}


void prueba2() {
	std::string toParse = "users id_user files :GET";
	char c_userID[100];
	sscanf(toParse.c_str(), "users %s files :GET", c_userID);
	std::cout << "Leyó de: " << toParse << " el id: " << c_userID << std::endl;
	toParse = "users :POST";
	sscanf(toParse.c_str(), "users %s files :GET", c_userID);
	std::cout << "Leyó de: " << toParse << " el id: " << c_userID << std::endl;
}


void prueba3() {
	std::string toParse = "/users/id_user/files:GET";
	std::smatch sm;
	if ( std::regex_match (toParse, sm, std::regex("(/users/)([^/]+)(/files:.*)")) ) {
		std::cout << "Matcheó. Los matches son: " << std::endl;
		for (unsigned i=0; i < sm.size(); ++i) {
			std::cout << i << "[" << sm[i].str() << "]" << std::endl;
		}
	} else {
		std::cout << "No matcheó." << std::endl;
	}
}


int main(){
	prueba1();
	//prueba2();
	//prueba3();
	return 0;
}
