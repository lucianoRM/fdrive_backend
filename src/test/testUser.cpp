
#include "googletest/include/gtest/gtest.h"
#include "user.h"
#include "folder.h"
#include "rocksdb/db.h"
#include "rocksdb/options.h"
#include <string.h>
#include "UserException.h"
#include "googletest/include/gtest/internal/gtest-internal.h"
#include "googletest/include/gtest/internal/gtest-port.h"
#include "googletest/include/gtest/gtest_pred_impl.h"

rocksdb::DB* openDatabase() {
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

TEST(HashedPasswordTest, HashedPassword) {
    rocksdb::DB* db = openDatabase();
    if (! db) {
    return;
    }
    User* user = new User();
    user->setEmail("emailTest");

    EXPECT_EQ("0c60c80f961f0e71f3a9b524af6012062fe037a6", user->hashPassword("password"));

    db->Delete(rocksdb::WriteOptions(), "users.emailTest");
    delete db;
    delete user;
}

TEST(SignUpTest, SignupAvailableEmail) {
    rocksdb::DB* db = openDatabase();
    if (! db) {
        return;
    }
    User* user = new User();
    user->setEmail("emailTest");
    user->setPassword("password");

    EXPECT_NO_THROW(user->signup(db));

    db->Delete(rocksdb::WriteOptions(), "users.emailTest");
    delete db;
    delete user;
}

TEST(SignUpTest, SignupTakenEmail){
    rocksdb::DB* db = openDatabase();
    if (! db) {
        return;
    }

    User* user = new User();
    user->setEmail("emailTest");
    user->setPassword("password");
    user->signup(db);

    User* user2= new User();
    user2->setEmail("emailTest");
    user2->setPassword("password");
    EXPECT_THROW(user2->signup(db), AlreadyExistentUserException);

    db->Delete(rocksdb::WriteOptions(), "users.emailTest");

    delete db;
    delete user;
    delete user2;
}

TEST(SignInTest, SignInExisting){
    rocksdb::DB* db = openDatabase();
    if (! db) {
        return;
    }

    User* user = new User();
    user->setEmail("emailTest");
    user->setPassword("password");
    user->signup(db);
    delete user;

    EXPECT_NO_THROW(user = User::load(db, "emailTest"));
    EXPECT_TRUE(user->login(std::string("password")));
    db->Delete(rocksdb::WriteOptions(), "users.emailTest");

    delete db;
    delete user;
}

TEST(SignInTest, SignInNonExisting){
    rocksdb::DB* db = openDatabase();
    if (! db) {
        return;
    }

    User* user;

    EXPECT_THROW(user = User::load(db, "emailTest"), NonExistentUserException);

    delete db;
}

TEST(SignInTest, SignInWrongPassword) {
    rocksdb::DB* db = openDatabase();
    if (! db) {
        return;
    }

    User* user = new User();
    user->setEmail("emailTest");
    user->setPassword("password");
    user->signup(db);
    delete user;

    user = User::load(db, "emailTest");
    EXPECT_FALSE(user->login(std::string("wrongpassword")));

    db->Delete(rocksdb::WriteOptions(), "users.emailTest");
    delete db;
    delete user;
}

TEST(TokensTest, DeleteExpiredTokens) {
    rocksdb::DB* db = openDatabase();
    if (! db) {
        return;
    }
    User* user = new User();
    user->setEmail("emailTest");
    user->setPassword("password");
    user->save(db);
    std::string token = user->getNewToken(db);
    EXPECT_NE("", token);

    EXPECT_NO_THROW(user->checkToken(token));

    time_t _currTime;
    time_t* currTime = &_currTime;
    time(currTime);
    *currTime += UserToken::TIME + 1;
    user->deleteExpiredTokens(currTime);

    EXPECT_THROW(user->checkToken(token), NotLoggedInException);
    delete db;
    delete user;
}

TEST(TokensTest, GetTwoDifferentTokens) {
    rocksdb::DB* db = openDatabase();
    if (! db) {
        return;
    }
    User* user = new User();
    std::string token1 = user->getNewToken(db);
    std::string token2 = user->getNewToken(db);

    EXPECT_NE(token1, token2);
    delete db;
    delete user;
}

TEST(TokensTest, CheckExistingToken) {
    rocksdb::DB* db = openDatabase();
    if (! db) {
        return;
    }
    User* user = new User();
    user->setEmail("a");
    std::string token1 = user->getNewToken(db);

    user = User::load(db, "a");
    EXPECT_NO_THROW(user->checkToken(token1));

    delete db;
    delete user;
}

TEST(TokensTest, CheckFirstOfManyTokens) {
    rocksdb::DB* db = openDatabase();
    if (! db) {
        return;
    }
    User* user = new User();
    user->setEmail("a");
    std::string token1 = user->getNewToken(db);
    user->getNewToken(db);
    user->getNewToken(db);
    user->getNewToken(db);
    user->getNewToken(db);
    user->getNewToken(db);

    user = User::load(db, "a");
    EXPECT_NO_THROW(user->checkToken(token1));

    delete db;
    delete user;
}

TEST(TokensTest, CheckLastOfManyTokens) {
    rocksdb::DB* db = openDatabase();
    if (! db) {
        return;
    }
    User* user = new User();
    user->setEmail("a");
    user->getNewToken(db);
    user->getNewToken(db);
    user->getNewToken(db);
    user->getNewToken(db);
    user->getNewToken(db);
    std::string token1 = user->getNewToken(db);

    user = User::load(db, "a");
    EXPECT_NO_THROW(user->checkToken(token1));

    delete db;
    delete user;
}

TEST(FilesTest, HasFileWhenFalse) {
    User* user = new User();
    user->setEmail("a");
    EXPECT_FALSE(user->hasFile(1));
    delete user;
}

TEST(FilesTest, HasFileWhenOwnerTrue) {
    User* user = new User();
    user->setEmail("a");
    user->addFile(1, "root");
    EXPECT_TRUE(user->hasFile(1));
    delete user;
}

TEST(FilesTest, HasFileWhenSharedTrue) {
    User* user = new User();
    user->setEmail("a");
    user->addSharedFile(1);
    EXPECT_TRUE(user->hasFile(1));
    delete user;
}

TEST(FilesTest, AddNewFileAndSaveInDatabase) {
    rocksdb::DB* db = openDatabase();
    if (! db) {
        return;
    }
    User* user = new User();
    user->setEmail("a");
    user->addFile(1, "root");
    user->save(db);
    delete user;

    user = User::load(db, "a");
    EXPECT_TRUE(user->hasFile(1));
    delete user;

    delete db;
}

TEST(FilesTest, AddNewSharedFileAndSaveInDatabase) {
    rocksdb::DB* db = openDatabase();
    if (! db) {
        return;
    }
    User* user = new User();
    user->setEmail("a");
    user->addSharedFile(1);
    user->save(db);
    delete user;

    user = User::load(db, "a");
    EXPECT_TRUE(user->hasFile(1));
    delete user;

    delete db;
}

TEST(FilesTest, AskForFileWhenFalseInDatabase) {
    rocksdb::DB* db = openDatabase();
    if (! db) {
        return;
    }
    User* user = new User();
    user->setEmail("a");
    user->save(db);
    delete user;

    user = User::load(db, "a");
    EXPECT_FALSE(user->hasFile(1));
    delete user;

    delete db;
}

std::string getEmptyJson() {
    Json::Value root;
    Json::StyledWriter writer;

    Json::Value folders;
    Json::Value files;

    root["folders"] = "";
    root["files"] = "";
    std::string json = writer.write(root);

    return json;
}

TEST(FolderTest, SavedFile) {
    rocksdb::DB* db = openDatabase();
    if (! db) {
        return;
    }

    std::string json = getEmptyJson();
    db->Put(rocksdb::WriteOptions(),"email.root",json);

    Folder* folder = new Folder();
    folder->load(db,"email","root");
    folder->addFile("file1");
    folder->save(db);

    Json::Reader reader;
    Json::Value root;

    std::string value;
    db->Get(rocksdb::ReadOptions(),"email.root",&value);
    if (! reader.parse(value,root,false)) throw;
    Json::Value files = root["files"];

    std::string file;
    for (Json::Value::iterator it = files.begin(); it != files.end();it++) {
        file = (*it).asString();
    }

    EXPECT_EQ("file1", file);

    db->Delete(rocksdb::WriteOptions(), "email.root");
    delete db;
    delete folder;
}

TEST(FolderTest, SavedFolder) {
    rocksdb::DB* db = openDatabase();
    if (! db) {
        return;
    }

    std::string json = getEmptyJson();
    db->Put(rocksdb::WriteOptions(),"email.root",json);

    Folder* folder = new Folder();
    folder->load(db,"email","root");
    folder->addFolder("folder1");
    folder->save(db);

    Json::Reader reader;
    Json::Value root;

    std::string value;
    db->Get(rocksdb::ReadOptions(),"email.root",&value);
    if (! reader.parse(value,root,false)) throw;
    Json::Value folders = root["folders"];

    std::string folderInside;
    for (Json::Value::iterator it = folders.begin(); it != folders.end();it++) {
        folderInside = (*it).asString();
    }

    EXPECT_EQ("folder1", folderInside);

    db->Delete(rocksdb::WriteOptions(), "email.root");
    delete db;
    delete folder;
}
