
#include "googletest/include/gtest/gtest.h"
#include "user.h"
#include "folder.h"
#include "rocksdb/db.h"
#include "rocksdb/options.h"
#include <string.h>
#include <iostream>
#include "userExceptions.h"
#include "folderExceptions.h"
#include "fileExceptions.h"
#include "googletest/include/gtest/internal/gtest-internal.h"
#include "googletest/include/gtest/internal/gtest-port.h"
#include "googletest/include/gtest/gtest_pred_impl.h"

rocksdb::DB* USER_openDatabase() {
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

void USER_deleteDatabase() {
    system("rm -rf userTestDB");
}

TEST(HashedPasswordTest, HashedPassword) {
    rocksdb::DB* db = USER_openDatabase();
    if (! db) {
    return;
    }
    User* user = new User();
    user->setEmail("emailTest");

    EXPECT_EQ("0c60c80f961f0e71f3a9b524af6012062fe037a6", user->hashPassword("password"));

    delete db;
    delete user;
    USER_deleteDatabase();
}

TEST(SignUpTest, SignupAvailableEmail) {
    rocksdb::DB* db = USER_openDatabase();
    if (! db) {
        return;
    }
    User* user = new User();
    user->setEmail("emailTest");
    user->setPassword("password");

    EXPECT_NO_THROW(user->signup(db));
    std::string value;
    rocksdb::Status status = db->Get(rocksdb::ReadOptions(), "users.emailTest", &value);
    EXPECT_FALSE(status.IsNotFound());
    status = db->Get(rocksdb::ReadOptions(), "users", &value);
    EXPECT_FALSE(status.IsNotFound());

    delete db;
    delete user;
    USER_deleteDatabase();
}

TEST(SignUpTest, SignupTakenEmail) {
    rocksdb::DB* db = USER_openDatabase();
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

    delete db;
    delete user;
    delete user2;
    USER_deleteDatabase();
}

TEST(LogInTest, LogInExisting) {
    rocksdb::DB* db = USER_openDatabase();
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

    delete db;
    delete user;
    USER_deleteDatabase();
}

TEST(LogInTest, LogInNonExisting) {
    rocksdb::DB* db = USER_openDatabase();
    if (! db) {
        return;
    }

    User* user = NULL;

    EXPECT_THROW(user = User::load(db, "emailTest"), NonExistentUserException);

    delete db;
    USER_deleteDatabase();
}

TEST(LogInTest, LogInWrongPassword) {
    rocksdb::DB* db = USER_openDatabase();
    User* user = new User();
    user->setEmail("emailTest");
    user->setPassword("password");
    user->signup(db);
    delete user;

    user = User::load(db, "emailTest");
    EXPECT_FALSE(user->login(std::string("wrongpassword")));

    delete db;
    delete user;
    USER_deleteDatabase();
}

TEST(LogOutTest, CorrectLogOutLeavesTokenInvalid) {
    rocksdb::DB* db = USER_openDatabase();
    
    User* user = new User();
    user->setEmail("emailTest");
    user->setPassword("password");
    user->signup(db);
    delete user;
    user = User::load(db, "emailTest");
    user->login(std::string("password"));
    std::string token = user->getNewToken(db);
    delete user;
    user = User::load(db, "emailTest");
    EXPECT_TRUE(user->logout(db, token));
    delete user;
    user = User::load(db, "emailTest");
    EXPECT_THROW(user->checkToken(token), NotLoggedInException);

    delete db;
    delete user;
    USER_deleteDatabase();
}

TEST(TokensTest, DeleteExpiredTokens) {
    rocksdb::DB* db = USER_openDatabase();
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
    USER_deleteDatabase();
}

TEST(TokensTest, GetTwoDifferentTokens) {
    rocksdb::DB* db = USER_openDatabase();
    if (! db) {
        return;
    }
    User* user = new User();
    std::string token1 = user->getNewToken(db);
    std::string token2 = user->getNewToken(db);

    EXPECT_NE(token1, token2);
    delete db;
    delete user;
    USER_deleteDatabase();
}

TEST(TokensTest, CheckExistingToken) {
    rocksdb::DB* db = USER_openDatabase();
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
    USER_deleteDatabase();
}

TEST(TokensTest, CheckFirstOfManyTokens) {
    rocksdb::DB* db = USER_openDatabase();
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
    USER_deleteDatabase();
}

TEST(TokensTest, CheckLastOfManyTokens) {
    rocksdb::DB* db = USER_openDatabase();
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
    USER_deleteDatabase();
}


TEST(LogOutTest, LogOutWithoutLogInFails) {
    rocksdb::DB* db = USER_openDatabase();

    User* user = new User();
    user->setEmail("emailTest");
    user->setPassword("password");
    user->signup(db);
    delete user;
    user = User::load(db, "emailTest");
    EXPECT_FALSE(user->logout(db, std::string("token")));

    delete db;
    delete user;
    USER_deleteDatabase();
}

TEST(LogOutTest, LogOutWithWrongTokenFails) {
    rocksdb::DB* db = USER_openDatabase();

    User* user = new User();
    user->setEmail("emailTest");
    user->setPassword("password");
    user->signup(db);
    delete user;
    user = User::load(db, "emailTest");
    user->login(std::string("password"));
    std::string token = user->getNewToken(db);
    delete user;
    std::string wrongToken = token + "WRONG";
    user = User::load(db, "emailTest");
    EXPECT_FALSE(user->logout(db, wrongToken));
    delete user;
    user = User::load(db, "emailTest");
    EXPECT_NO_THROW(user->checkToken(token));

    delete db;
    delete user;
    USER_deleteDatabase();
}

TEST(NameTest, NoNameInDatabaseAtStart) {
    rocksdb::DB* db = USER_openDatabase();

    User* user = new User();
    user->setEmail("emailTest");
    user->setPassword("password");
    user->signup(db);
    delete user;
    std::string json;
    rocksdb::Status status = db->Get(rocksdb::ReadOptions(), "users.emailTest", &json);
    EXPECT_FALSE(json.find("name") != std::string::npos);
    delete db;
    USER_deleteDatabase();
}

TEST(NameTest, AddedNameToDatabase) {
    rocksdb::DB* db = USER_openDatabase();

    User* user = new User();
    user->setEmail("emailTest");
    user->setPassword("password");
    user->setName("Nombre De Usuario");
    user->signup(db);
    delete user;
    std::string json;
    rocksdb::Status status = db->Get(rocksdb::ReadOptions(), "users.emailTest", &json);
    Json::Reader reader;
    Json::Value root;
    EXPECT_TRUE(reader.parse(json, root, false));
    EXPECT_EQ("Nombre De Usuario", root["name"].asString());

    delete db;
    USER_deleteDatabase();
}


TEST(LocationTest, NoLastLocationInDatabaseAtStart) {
    rocksdb::DB* db = USER_openDatabase();

    User* user = new User();
    user->setEmail("emailTest");
    user->setPassword("password");
    user->signup(db);
    delete user;
    std::string json;
    rocksdb::Status status = db->Get(rocksdb::ReadOptions(), "users.emailTest", &json);
    EXPECT_FALSE(json.find("lastLocation") != std::string::npos);
    delete db;
    USER_deleteDatabase();
}

TEST(LocationTest, AddedLocationToDatabase) {
    rocksdb::DB* db = USER_openDatabase();

    User* user = new User();
    user->setEmail("emailTest");
    user->setPassword("password");
    user->setLocation("Location");
    user->signup(db);
    delete user;
    std::string json;
    rocksdb::Status status = db->Get(rocksdb::ReadOptions(), "users.emailTest", &json);
    Json::Reader reader;
    Json::Value root;
    EXPECT_TRUE(reader.parse(json, root, false));
    EXPECT_EQ("Location", root["lastLocation"].asString());

    delete db;
    USER_deleteDatabase();
}


TEST(ProfilePictureTest, NoPictureInDatabaseAtStart) {
    rocksdb::DB* db = USER_openDatabase();

    User* user = new User();
    user->setEmail("emailTest");
    user->setPassword("password");
    user->signup(db);
    delete user;
    std::string json;
    rocksdb::Status status = db->Get(rocksdb::ReadOptions(), "users.emailTest", &json);
    EXPECT_FALSE(json.find("pathToProfilePicture") != std::string::npos);
    delete db;
    USER_deleteDatabase();
}

TEST(ProfilePictureTest, AddedPictureToDatabase) {
    rocksdb::DB* db = USER_openDatabase();

    User* user = new User();
    user->setEmail("emailTest");
    user->setPassword("password");
    user->setProfilePicturePath("Picture Path");
    user->signup(db);
    delete user;
    std::string json;
    rocksdb::Status status = db->Get(rocksdb::ReadOptions(), "users.emailTest", &json);
    Json::Reader reader;
    Json::Value root;
    EXPECT_TRUE(reader.parse(json, root, false));
    EXPECT_EQ("Picture Path", root["pathToProfilePicture"].asString());

    delete db;
    USER_deleteDatabase();
}



