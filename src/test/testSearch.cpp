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

rocksdb::DB* openDatabaseSearch() {
    rocksdb::DB* db;
    rocksdb::Options options;
    options.create_if_missing = true;
    rocksdb::Status status = rocksdb::DB::Open(options, "userTestDB", &db);

    //La db se abrio correctamente
    if (!status.ok()){
        std::cout << "Error DB:" << status.ToString() << std::endl;
        return NULL;
    }

    return db;
}

File* generateNewFile(rocksdb::DB* db) {
    File* file = new File();
    file->setName("file1");
    file->setExtension(".doc");
    file->setOwner("email");
    file->setOwnerPath("root");
    file->setTag("tag");
    file->setId(1);
    file->save(db);
    file->saveSearches("email", "root", db);

    return file;
}

TEST(SearchTest, TagFileSearch) {
    rocksdb::DB* db = openDatabaseSearch();
    if (! db) {
        return;
    }

    File* file = generateNewFile(db);

    std::string value;
    db->Get(rocksdb::ReadOptions(), "searchtag.email.tag", &value);
    Json::Reader reader;
    Json::Value jsonFiles;

    if (! reader.parse(value, jsonFiles, false)) throw;
    Json::Value::iterator it = jsonFiles["files"].begin();
    int id = (*it)["id"].asInt();
    std::string path = (*it)["path"].asString();


    EXPECT_EQ(1,id);
    EXPECT_EQ("root",path);

    db->Delete(rocksdb::WriteOptions(), "searchtag.email.tag");
    db->Delete(rocksdb::WriteOptions(),"files.1");
    delete db;
    delete file;
}

TEST(SearchTest, OwnerFileSearch) {
    rocksdb::DB* db = openDatabaseSearch();
    if (! db) {
        return;
    }

    File* file = generateNewFile(db);

    std::string value;
    db->Get(rocksdb::ReadOptions(), "searchowner.email.email", &value);
    Json::Reader reader;
    Json::Value jsonFiles;

    if (! reader.parse(value, jsonFiles, false)) throw;
    Json::Value::iterator it = jsonFiles["files"].begin();
    int id = (*it)["id"].asInt();
    std::string path = (*it)["path"].asString();


    EXPECT_EQ(1,id);
    EXPECT_EQ("root",path);

    db->Delete(rocksdb::WriteOptions(), "searchowner.email.email");
    db->Delete(rocksdb::WriteOptions(),"files.1");
    delete db;
    delete file;
}

TEST(SearchTest, NameFileSearch) {
    rocksdb::DB* db = openDatabaseSearch();
    if (! db) {
        return;
    }

    File* file = generateNewFile(db);

    std::string value;
    db->Get(rocksdb::ReadOptions(), "searchname.email.file1", &value);
    Json::Reader reader;
    Json::Value jsonFiles;

    if (! reader.parse(value, jsonFiles, false)) throw;
    Json::Value::iterator it = jsonFiles["files"].begin();
    int id = (*it)["id"].asInt();
    std::string path = (*it)["path"].asString();


    EXPECT_EQ(1,id);
    EXPECT_EQ("root",path);

    db->Delete(rocksdb::WriteOptions(), "searchname.email.file1");
    db->Delete(rocksdb::WriteOptions(),"files.1");
    delete db;
    delete file;
}




