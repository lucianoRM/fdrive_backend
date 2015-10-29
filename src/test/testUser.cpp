
#include "../include/googletest/include/gtest/gtest.h"
#include "../user.h"
#include "../include/rocksdb/db.h"
#include "../include/rocksdb/options.h"
#include <string.h>
#include "../UserException.h"
#include "../include/googletest/include/gtest/internal/gtest-internal.h"
#include "../include/googletest/include/gtest/internal/gtest-port.h"
#include "../include/googletest/include/gtest/gtest_pred_impl.h"

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
    user->addFile(1);
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
    user->addFile(1);
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