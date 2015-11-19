#ifndef TALLER_SEARCH_H
#define TALLER_SEARCH_H

#include <string.h>
#include <list>
#include "rocksdb/db.h"
#include "json/json.h"
#include "json/json-forwards.h"
#include <iostream>
#include "exceptions/searchExceptions.h"
#include "exceptions/dbExceptions.h"


// Struct containing the information of each file
// that must be stored to complete a search from a user.
typedef struct searchFile {
    int id;
    std::string path;
} searchFile;


// Class that represents all files available to a user that
// fits a search.
class SearchInformation {

    private:
        std::list<struct searchFile*>* files;
        std::string typeOfSearch;
        std::string user;
        std::string element;
        Json::Value getJson();

    public:

        SearchInformation();
        ~SearchInformation();

        // Type of search must be in lower case and it should be: tag, owner, name or extension.
        static SearchInformation* load(rocksdb::DB* db, std::string typeOfSearch, std::string user, std::string element);
        void save(rocksdb::DB* db);

        void addFile(int id, std::string path);
    /* Supone que no hay copias de un mismo archivo en distintos paths.*/
        void eraseFile(int id);

        std::string getContent();
        void changeElement(int id, std::string element);
        std::string getUserPath(int id);

};

#endif //TALLER_SEARCH_H