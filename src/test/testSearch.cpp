//
// Created by martin on 6/11/15.
//

#include "googletest/include/gtest/gtest.h"
#include "googletest/include/gtest/internal/gtest-internal.h"
#include "googletest/include/gtest/internal/gtest-port.h"
#include "googletest/include/gtest/gtest_pred_impl.h"

#include "rocksdb/db.h"
#include "rocksdb/options.h"
#include <string.h>
#include <iostream>
#include <searchInformation/searchInformation.h>
#include <file/file.h>

#include "fileExceptions.h"

rocksdb::DB* SEARCH_openDatabase() {
    rocksdb::DB* db;
    rocksdb::Options options;
    options.create_if_missing = true;
    rocksdb::Status status = rocksdb::DB::Open(options, "searchTestDB", &db);

    //La db se abrio correctamente
    if (!status.ok()){
        std::cout << "Error DB:" << status.ToString() << std::endl;
        return NULL;
    }

    return db;
}

void SEARCH_deleteDatabase() {
    system("rm -rf userTestDB");
}

File* generateNewFile(rocksdb::DB* db) {
    File* file = new File();
    file->setName("file1");
    file->setExtension("doc");
    file->setOwner("email");
    file->setOwnerPath("root");
    file->setTag("tag");
    file->genId(db);

    return file;
}

TEST(SearchTest, TagFileSearch) {
    rocksdb::DB* db = SEARCH_openDatabase();
    if (! db) {
        return;
    }

    File* file = generateNewFile(db);
    file->save(db);

    file->saveSearches("email", "root", db);

    std::string value;
    db->Get(rocksdb::ReadOptions(), "searchtag.email.tag", &value);
    Json::Reader reader;
    Json::Value jsonFiles;

    if (! reader.parse(value, jsonFiles, false)) throw;
    Json::Value::iterator it = jsonFiles["files"].begin();
    int id = (*it)["id"].asInt();
    std::string path = (*it)["path"].asString();


    EXPECT_EQ(0,id);
    EXPECT_EQ("root",path);

    delete db;
    delete file;
    SEARCH_deleteDatabase();
}

TEST(SearchTest, OwnerFileSearch) {
    rocksdb::DB* db = SEARCH_openDatabase();
    if (! db) {
        return;
    }

    File* file = generateNewFile(db);
    file->save(db);
    file->saveSearches("email", "root", db);

    std::string value;
    rocksdb::Status status = db->Get(rocksdb::ReadOptions(), "searchowner.email.email", &value);
    Json::Reader reader;
    Json::Value jsonFiles;

    if (! reader.parse(value, jsonFiles, false)) throw;
    Json::Value::iterator it = jsonFiles["files"].begin();
    int id = (*it)["id"].asInt();
    std::string path = (*it)["path"].asString();


    EXPECT_EQ(0,id);
    EXPECT_EQ("root",path);

    delete db;
    delete file;
    SEARCH_deleteDatabase();
}

TEST(SearchTest, NameFileSearch) {
    rocksdb::DB* db = SEARCH_openDatabase();
    if (! db) {
        return;
    }

    File* file = generateNewFile(db);
    file->save(db);
    file->saveSearches("email", "root", db);

    std::string value;
    db->Get(rocksdb::ReadOptions(), "searchname.email.file1", &value);
    Json::Reader reader;
    Json::Value jsonFiles;

    if (! reader.parse(value, jsonFiles, false)) throw;
    Json::Value::iterator it = jsonFiles["files"].begin();
    int id = (*it)["id"].asInt();
    std::string path = (*it)["path"].asString();


    EXPECT_EQ(0,id);
    EXPECT_EQ("root",path);

    delete db;
    delete file;
    SEARCH_deleteDatabase();
}

TEST(SearchTest, ExtensionFileSearch) {
    rocksdb::DB* db = SEARCH_openDatabase();
    if (! db) {
        return;
    }

    File* file = generateNewFile(db);
    file->save(db);
    file->saveSearches("email", "root", db);

    std::string value;
    db->Get(rocksdb::ReadOptions(), "searchname.email.file1", &value);
    Json::Reader reader;
    Json::Value jsonFiles;

    if (! reader.parse(value, jsonFiles, false)) throw;
    Json::Value::iterator it = jsonFiles["files"].begin();
    int id = (*it)["id"].asInt();
    std::string path = (*it)["path"].asString();


    EXPECT_EQ(0,id);
    EXPECT_EQ("root",path);

    delete db;
    delete file;
    SEARCH_deleteDatabase();
}

TEST(SearchTest, TwoFilesWithSameOwner) {
    rocksdb::DB* db = SEARCH_openDatabase();
    if (! db) {
        return;
    }

    File* file1 = generateNewFile(db);
    file1->save(db);
    file1->saveSearches("email", "root", db);

    File* file2 = generateNewFile(db);
    file2->save(db);
    file2->saveSearches("email", "root/other", db);

    std::string value;
    db->Get(rocksdb::ReadOptions(), "searchname.email.file1", &value);
    Json::Reader reader;
    Json::Value jsonFiles;

    if (! reader.parse(value, jsonFiles, false)) throw;

    std::list<struct searchFile*>* files = new std::list<struct searchFile*>();
    for (Json::Value::iterator it = jsonFiles["files"].begin(); it != jsonFiles["files"].end(); it++) {
        struct searchFile* file = new searchFile;
        file->id = (*it)["id"].asInt();
        file->path = (*it)["path"].asString();
        files->push_back(file);
    }

    searchFile* fileSearch1 = files->pop_front();
    searchFile* fileSearch2 = files->pop_front();

    EXPECT_EQ(0,fileSearch1->i);
    EXPECT_EQ("root",fileSearch1->path);
    EXPECT_EQ(1,fileSearch2->i);
    EXPECT_EQ("root/other",fileSearch2->path);


    delete db;
    delete file1;
    delete file2;
    SEARCH_deleteDatabase();
}

