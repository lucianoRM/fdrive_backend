#include "googletest/include/gtest/gtest.h"
#include "googletest/include/gtest/internal/gtest-internal.h"
#include "googletest/include/gtest/internal/gtest-port.h"
#include "googletest/include/gtest/gtest_pred_impl.h"

#include "file.h"
#include "fileExceptions.h"


rocksdb::DB* FILE_openDatabase() {
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

void FILE_deleteDatabase() {
    system("rm -rf userTestDB");
}

TEST(FileCreationTest, CreateFileWithBasicInformation) {
    rocksdb::DB* db = FILE_openDatabase();

    File* file = new File();
    file->setId(4);
    file->setName("Nombre");
    file->setExtension("ext");
    file->setOwner("Owner");
    file->setOwnerPath("root");
    file->setLastUser("OtherUser");
    file->save(db);
    delete file;
    std::string json;
    rocksdb::Status status = db->Get(rocksdb::ReadOptions(), "files.4", &json);
    Json::Reader reader;
    Json::Value root;
    EXPECT_TRUE(reader.parse(json, root, false));
    EXPECT_EQ("Nombre", root["name"].asString());
    EXPECT_EQ("ext", root["extension"].asString());
    EXPECT_EQ("Owner", root["owner"].asString());
    EXPECT_EQ("root", root["pathInOwner"].asString());
    EXPECT_EQ("OtherUser", root["lastUser"].asString());
    EXPECT_TRUE(root.isMember("lastModified")); //Deberíamos chequear también el valor.
    EXPECT_TRUE(root.isMember("users"));

    delete db;
    FILE_deleteDatabase();
}

TEST(FileCreationTest, LoadFile) {
    rocksdb::DB* db = FILE_openDatabase();

    File* file = new File();
    file->genId(db);
    file->setName("Nombre");
    file->setExtension("ext");
    file->setOwner("Owner");
    file->setOwnerPath("root");
    file->setLastUser("OtherUser");
    file->save(db);
    delete file;
    file = File::load(db, 0);
    Json::Value json = file->getJson();
    EXPECT_EQ("Nombre", json["name"].asString());
    EXPECT_EQ("ext", json["extension"].asString());
    EXPECT_EQ("Owner", json["owner"].asString());
    EXPECT_EQ("root", json["pathInOwner"].asString());
    EXPECT_EQ("OtherUser", json["lastUser"].asString());
    EXPECT_TRUE(json.isMember("lastModified")); //Deberíamos chequear también el valor.
    EXPECT_TRUE(json.isMember("users"));

    delete db;
    FILE_deleteDatabase();
}


TEST(IdGenerationTest, GeneratingFromFile) {
    rocksdb::DB* db = FILE_openDatabase();

    File* file = new File();
    file->genId(db);
    EXPECT_EQ(0, file->getId());
    file->genId(db);
    EXPECT_EQ(1, file->getId());
    file->genId(db);
    file->genId(db);
    file->genId(db);
    EXPECT_EQ(4, file->getId());

    delete file;
    delete db;
    FILE_deleteDatabase();
}

TEST(IdGenerationTest, TouchingDatabase) {
    rocksdb::DB* db = FILE_openDatabase();

    File* file = new File();
    file->genId(db);
    EXPECT_EQ(0, file->getId());
    std::string value;
    rocksdb::Status status = db->Get(rocksdb::ReadOptions(), "files.maxID", &value);
    EXPECT_EQ(0, atoi(value.c_str()));
    status = db->Put(rocksdb::WriteOptions(), "files.maxID", "7");
    file->genId(db);
    EXPECT_EQ(8, file->getId());
    file->genId(db);
    EXPECT_EQ(9, file->getId());

    delete file;
    delete db;
    FILE_deleteDatabase();
}

