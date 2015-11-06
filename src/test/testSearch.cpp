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
#include <file/file.h>
#include <searchInformation/searchInformation.h>

#include "fileExceptions.h"

TEST(SearchTest, NewFileSearch) {
    rocksdb::DB* db = openDatabase();
    if (! db) {
        return;
    }

    File* file = new File();
    file->setName("file1");
    file->setExtension(".doc");
    file->setOwner("email");
    file->setOwnerPath("root");
    file->setTag("tag");
    file->setId(1);
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


    EXPECT_EQ(1,id);
    EXPECT_EQ("root",path);

    db->Delete(rocksdb::WriteOptions(), "searchname.email.file1");
    delete db;
    delete file;
}