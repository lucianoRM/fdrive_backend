//
// Created by martin on 13/11/15.
//

#include <user/user.h>
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
    folder->addFolder("folder1",db);
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
    folder->addFolder("folder",db);
    folder->save(db);

    EXPECT_THROW(folder->addFolder("folder",db),AlreadyExistentFolderException);

    delete db;
    delete folder;
    FOLDER_deleteDatabase();
}

TEST(FolderTest, RenameFolder) {
    rocksdb::DB* db = FOLDER_openDatabase();
    if (! db) {
        return;
    }

    std::string json = getEmptyJson();
    db->Put(rocksdb::WriteOptions(),"email.root",json);

    Folder* folder = Folder::load(db,"email","root");
    folder->addFolder("folderOld",db);
    folder->save(db);
    delete folder;

    folder = Folder::load(db,"email","root");
    folder->renameFolder("folderOld","folderNew");
    folder->save(db);
    delete folder;

    folder = Folder::load(db,"email","root/folderOld");
    folder->renamePath("root/folderNew");
    folder->save(db);
    delete folder;

    Json::Reader reader;
    Json::Value root;

    std::string value;
    db->Get(rocksdb::ReadOptions(),"email.root",&value);
    if (! reader.parse(value,root,false)) throw;

    Json::Value folders = root["folders"];
    Json::Value::iterator it = folders.begin();
    std::string folderInside = (*it).asString();

    EXPECT_EQ(true,true);

    delete db;
    FOLDER_deleteDatabase();
}

TEST(FolderTest, RemoveFile) {
    rocksdb::DB* db = FOLDER_openDatabase();
    if (! db) {
        return;
    }

    User* user = new User();
    user->setEmail("owner");
    user->setPassword("password");
    user->signup(db);
    user = user->load(db,"owner");

    File* file = new File();
    file->genId(db);
    file->setName("Nombre");
    file->setExtension(".ext");
    file->setOwner("owner");
    file->setOwnerPath("root");
    file->setLastUser("owner");
    file->save(db);
    file->saveSearches("owner","root",db);
    int id = file->getId();
    delete file;

    Folder* folder = Folder::load(db,"owner","root");
    folder->addFile(id,"Nombre.ext");
    folder->save(db);

    file = File::load(db,id);
    file->eraseFromUser(db,"owner","root");
    file->save(db);
    file->saveSearches("owner","trash",db);

    folder = Folder::load(db,"owner","root");
    std::string json = folder->getContent();
    Json::Reader reader;
    Json::Value root;

    std::cout << "El json es: " << json << std::endl;

    EXPECT_TRUE(reader.parse(json, root, false));
    Json::Value files = root["files"];
    for (Json::Value::iterator it = files.begin(); it != files.end();it++) {
        std::cout << "\n TIENE: " << it->asString();
        EXPECT_NE(it->asInt(),id);
    }

    Folder* folderTrash = Folder::load(db,"owner","trash");
    json = folderTrash->getContent();

    std::cout << "El json es: " << json << std::endl;

    EXPECT_TRUE(reader.parse(json, root, false));
    files = root["files"];
    for (Json::Value::iterator it = files.begin(); it != files.end();it++) {
        std::cout << "\n TIENE: " << it->asString();
    }

    delete db;
    delete file;
    delete folder;
    delete folderTrash;
    FOLDER_deleteDatabase();
}