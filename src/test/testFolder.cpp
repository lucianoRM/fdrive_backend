//
// Created by martin on 13/11/15.
//

#include "googletest/include/gtest/gtest.h"
#include "googletest/include/gtest/internal/gtest-internal.h"
#include "googletest/include/gtest/internal/gtest-port.h"
#include "googletest/include/gtest/gtest_pred_impl.h"

#include "file.h"
#include "fileExceptions.h"
#include "folder.h"
#include "folderExceptions.h"

rocksdb::DB* FOLDER_openDatabase() {
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

void FOLDER_deleteDatabase() {
    system("rm -rf userTestDB");
}

std::string getEmptyJson() {
    Json::Value root;
    Json::StyledWriter writer;

    Json::Value folders (Json::arrayValue);
    Json::Value files (Json::arrayValue);

    root["folders"] = folders;
    root["files"] = files;
    std::string json = writer.write(root);

    return json;
}

TEST(FolderTest, SavedFile) {
    rocksdb::DB* db = FOLDER_openDatabase();
    if (! db) {
        return;
    }

    std::string json = getEmptyJson();
    db->Put(rocksdb::WriteOptions(),"email.root",json);

    Folder* folder = Folder::load(db,"email","root");
    folder->addFile(1,"file1");
    folder->save(db);

    Json::Reader reader;
    Json::Value root;

    std::string value;
    db->Get(rocksdb::ReadOptions(),"email.root",&value);
    if (! reader.parse(value,root,false)) throw;
    Json::Value files = root["files"];

    Json::Value::iterator it = files.begin();
    int file = (*it).asInt();

    EXPECT_EQ(1, file);

    delete db;
    delete folder;
    FOLDER_deleteDatabase();
}

TEST(FolderTest, SavedFolder) {
    rocksdb::DB* db = FOLDER_openDatabase();
    if (! db) {
        return;
    }

    std::string json = getEmptyJson();
    db->Put(rocksdb::WriteOptions(),"email.root",json);

    Folder* folder = Folder::load(db,"email","root");
    folder->addFolder("folder1");
    folder->save(db);

    Json::Reader reader;
    Json::Value root;

    std::string value;
    db->Get(rocksdb::ReadOptions(),"email.root",&value);
    if (! reader.parse(value,root,false)) throw;

    Json::Value folders = root["folders"];
    Json::Value::iterator it = folders.begin();
    std::string folderInside = (*it).asString();

    EXPECT_EQ("folder1", folderInside);

    delete db;
    delete folder;
    FOLDER_deleteDatabase();
}

TEST(FolderTest, AlreadyExistentFile) {
    rocksdb::DB* db = FOLDER_openDatabase();
    if (! db) {
        return;
    }

    std::string json = getEmptyJson();
    db->Put(rocksdb::WriteOptions(),"email.root",json);

    Folder* folder = Folder::load(db,"email","root");
    folder->addFile(1,"file1");
    folder->save(db);


    EXPECT_THROW(folder->addFile(2,"file1"),FilenameTakenException);

    delete db;
    delete folder;
    FOLDER_deleteDatabase();
}

TEST(FolderTest, AlreadyExistentFolder) {
    rocksdb::DB* db = FOLDER_openDatabase();
    if (! db) {
        return;
    }

    std::string json = getEmptyJson();
    db->Put(rocksdb::WriteOptions(),"email.root",json);

    Folder* folder = Folder::load(db,"email","root");
    folder->addFolder("folder");
    folder->save(db);

    EXPECT_THROW(folder->addFolder("folder"),AlreadyExistentFolderException);

    delete db;
    delete folder;
    FOLDER_deleteDatabase();
}
