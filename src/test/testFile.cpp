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
    
    //std::cout << "El json es: " << json << std::endl;
    
    EXPECT_TRUE(reader.parse(json, root, false));
    EXPECT_EQ("Owner", root["owner"].asString());
    EXPECT_TRUE(root.isMember("lastVersion")); 
    EXPECT_EQ(0, root["lastVersion"].asInt());
    EXPECT_TRUE(root.isMember("users")); 
    EXPECT_TRUE(root["users"].begin() == root["users"].end());
    EXPECT_TRUE(root.isMember("versions"));

    EXPECT_TRUE(root["versions"].isMember("0"));
    EXPECT_FALSE(root["versions"].isMember("1"));
    EXPECT_EQ("Nombre", root["versions"]["0"]["name"].asString());
    EXPECT_EQ("ext", root["versions"]["0"]["extension"].asString());
    EXPECT_EQ("root", root["versions"]["0"]["pathInOwner"].asString());
    EXPECT_EQ("OtherUser", root["versions"]["0"]["lastUser"].asString());
    EXPECT_TRUE(root["versions"]["0"].isMember("lastModified")); //Deberíamos chequear también el valor.

    //delete db;
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
    
    //std::cout << "El json es: " << json << std::endl;
    
    EXPECT_EQ("Owner", json["owner"].asString());
    EXPECT_TRUE(json.isMember("lastVersion")); 
    EXPECT_EQ(0, json["lastVersion"].asInt());
    EXPECT_TRUE(json.isMember("users")); 
    EXPECT_TRUE(json["users"].begin() == json["users"].end());
    EXPECT_EQ("Nombre", json["name"].asString());
    EXPECT_EQ("ext", json["extension"].asString());
    EXPECT_EQ("root", json["pathInOwner"].asString());
    EXPECT_EQ("OtherUser", json["lastUser"].asString());
    EXPECT_TRUE(json.isMember("lastModified"));	//Deberíamos chequear también el valor.

    //delete db;
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
    //delete db;
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
    //delete db;
    FILE_deleteDatabase();
}

TEST(VersionTest, TwoVersionsThenSave) {
	rocksdb::DB* db = FILE_openDatabase();

    File* file = new File();
    file->setId(4);
    file->setName("Nombre");
    file->setExtension("ext");
    file->setOwner("Owner");
    file->setOwnerPath("root");
    file->setLastUser("OtherUser");
    
    file->startNewVersion();
    file->setName("Nombre2");
    file->setExtension("ext2");
    // El owner no cambia en toda la vida del archivo se supone.
    file->setOwnerPath("root/files");
    file->setLastUser("OtherUser2");
    
    file->save(db);
    delete file;
    
    std::string json;
    rocksdb::Status status = db->Get(rocksdb::ReadOptions(), "files.4", &json);
    Json::Reader reader;
    Json::Value root;
    
    //std::cout << "El json es: " << json << std::endl;
    
    EXPECT_TRUE(reader.parse(json, root, false));
    EXPECT_EQ("Owner", root["owner"].asString());
    EXPECT_TRUE(root.isMember("lastVersion")); 
    EXPECT_EQ(1, root["lastVersion"].asInt());
    EXPECT_TRUE(root.isMember("users")); 
    EXPECT_TRUE(root["users"].begin() == root["users"].end());
    EXPECT_TRUE(root.isMember("versions"));

    EXPECT_TRUE(root["versions"].isMember("0"));
    EXPECT_EQ("Nombre", root["versions"]["0"]["name"].asString());
    EXPECT_EQ("ext", root["versions"]["0"]["extension"].asString());
    EXPECT_EQ("root", root["versions"]["0"]["pathInOwner"].asString());
    EXPECT_EQ("OtherUser", root["versions"]["0"]["lastUser"].asString());
    EXPECT_TRUE(root["versions"]["0"].isMember("lastModified")); //Deberíamos chequear también el valor.

	EXPECT_TRUE(root["versions"].isMember("1"));
	EXPECT_EQ("Nombre2", root["versions"]["1"]["name"].asString());
    EXPECT_EQ("ext2", root["versions"]["1"]["extension"].asString());
    EXPECT_EQ("root/files", root["versions"]["1"]["pathInOwner"].asString());
    EXPECT_EQ("OtherUser2", root["versions"]["1"]["lastUser"].asString());
    EXPECT_TRUE(root["versions"]["1"].isMember("lastModified")); //Deberíamos chequear también el valor.

    //delete db;
    FILE_deleteDatabase();
}


TEST(VersionTest, OneVersionSaveThenSecondAndSave) {
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
    
    file = File::load(db, 4);
    file->startNewVersion();
    file->setName("Nombre2");
    file->setExtension("ext2");
    // El owner no cambia en toda la vida del archivo se supone. Asique si le cambia, le cambia a todas las versiones.
    file->setOwnerPath("root/files");
    file->setLastUser("OtherUser2");
    file->save(db);
    delete file;
    
    std::string json;
    rocksdb::Status status = db->Get(rocksdb::ReadOptions(), "files.4", &json);
    Json::Reader reader;
    Json::Value root;
    
    //std::cout << "El json es: " << json << std::endl;
    
    EXPECT_TRUE(reader.parse(json, root, false));
    EXPECT_EQ("Owner", root["owner"].asString());
    EXPECT_TRUE(root.isMember("lastVersion")); 
    EXPECT_EQ(1, root["lastVersion"].asInt());
    EXPECT_TRUE(root.isMember("users")); 
    EXPECT_TRUE(root["users"].begin() == root["users"].end());
    EXPECT_TRUE(root.isMember("versions"));

    EXPECT_TRUE(root["versions"].isMember("0"));
    EXPECT_EQ("Nombre", root["versions"]["0"]["name"].asString());
    EXPECT_EQ("ext", root["versions"]["0"]["extension"].asString());
    EXPECT_EQ("root", root["versions"]["0"]["pathInOwner"].asString());
    EXPECT_EQ("OtherUser", root["versions"]["0"]["lastUser"].asString());
    EXPECT_TRUE(root["versions"]["0"].isMember("lastModified")); //Deberíamos chequear también el valor.

	EXPECT_TRUE(root["versions"].isMember("1"));
	EXPECT_EQ("Nombre2", root["versions"]["1"]["name"].asString());
    EXPECT_EQ("ext2", root["versions"]["1"]["extension"].asString());
    EXPECT_EQ("root/files", root["versions"]["1"]["pathInOwner"].asString());
    EXPECT_EQ("OtherUser2", root["versions"]["1"]["lastUser"].asString());
    EXPECT_TRUE(root["versions"]["1"].isMember("lastModified")); //Deberíamos chequear también el valor.

    //delete db;
    FILE_deleteDatabase();
}

TEST(VersionTest, LoadVersionAndChangeData) {
	rocksdb::DB* db = FILE_openDatabase();

    File* file = new File();
    file->genId(db); //Queda id = 0.
    file->setName("Nombre");
    file->setExtension("ext");
    file->setOwner("Owner");
    file->setOwnerPath("root");
    file->setLastUser("OtherUser");
    file->save(db);
    delete file;
    
    file = File::load(db, 0);
    file->setName("Nombre2");
    file->setExtension("ext2");
    // El owner no cambia en toda la vida del archivo se supone. Asique si le cambia, le cambia a todas las versiones.
    file->setOwnerPath("root/files");
    file->setLastUser("OtherUser2");
    file->save(db);
    delete file;
    
    std::string json;
    rocksdb::Status status = db->Get(rocksdb::ReadOptions(), "files.0", &json);
    Json::Reader reader;
    Json::Value root;
    
    //std::cout << "El json es: " << json << std::endl;
    
    EXPECT_TRUE(reader.parse(json, root, false));
    EXPECT_EQ("Owner", root["owner"].asString());
    EXPECT_TRUE(root.isMember("lastVersion")); 
    EXPECT_EQ(0, root["lastVersion"].asInt());
    EXPECT_TRUE(root.isMember("users")); 
    EXPECT_TRUE(root["users"].begin() == root["users"].end());
    EXPECT_TRUE(root.isMember("versions"));

    EXPECT_TRUE(root["versions"].isMember("0"));
    EXPECT_EQ("Nombre2", root["versions"]["0"]["name"].asString());
    EXPECT_EQ("ext2", root["versions"]["0"]["extension"].asString());
    EXPECT_EQ("root/files", root["versions"]["0"]["pathInOwner"].asString());
    EXPECT_EQ("OtherUser2", root["versions"]["0"]["lastUser"].asString());
    EXPECT_TRUE(root["versions"]["0"].isMember("lastModified")); //Deberíamos chequear también el valor.

    
    //delete db;
    FILE_deleteDatabase();
}
