#include <string>
#include <iostream>
#include "jsoncpp/json/json.h"

bool prueba1(){
	std::string toParse = "{\"email\":\"a@m.com\",\"password\":\"pass\"}";
	
	Json::Reader reader;
    Json::Value root;
    bool parsingSuccessful = reader.parse(toParse, root, false); // False for ignoring comments.
    if (!parsingSuccessful){
		std::cout << "JsonCPP no pudo parsear en prueba1." << std::endl;
		return false;
	}
    std::string pass = root.get("password", "").asString();
	std::cout << "Password es: " << pass << std::endl;
	return true;
}

bool prueba2(){
	float tiempo = 20;

	std::string toParse = "{\"tokens\":[{\"token\":\"sde3g\",\"expiration\":10},{\"token\":\"44j\",\"expiration\":20},{\"token\":\"kkjdh\",\"expiration\":30}]}";
	
	Json::Reader reader;
    Json::Value root;
    bool parsingSuccessful = reader.parse(toParse, root, false); // False for ignoring comments.
    if (!parsingSuccessful){
		std::cout << "JsonCPP no pudo parsear en prueba2." << std::endl;
		return false;
	}
	Json::Value newTokens;
	if (root.isMember("tokens")){
		Json::Value tokens = root["tokens"];
		for (size_t i = 0; i < tokens.size(); i++){
			float tiempoToken = tokens[(int)i]["expiration"].asFloat();
			if (tiempoToken > tiempo) newTokens.append(tokens[(int)i]);
		}
	}
	Json::Value jsonToken;
	jsonToken["token"] = "NuevoToken";
	jsonToken["expiration"] = 40;
	newTokens.append(jsonToken);
	root["tokens"] = newTokens;
	
	//~ std::cout << "Nuevo Json: " << root << std::endl;
	Json::StyledWriter writer;
	std::cout << "Nuevo Json: " << writer.write(root) << std::endl;
	std::cout << "Solo newTokens: " << writer.write(newTokens) << std::endl;
	return true;
}

int main(){
	prueba1();
	prueba2();
	return 0;
}
