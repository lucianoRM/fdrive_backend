//
// Created by luciano on 01/10/15.
//

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
    EXPECT_TRUE(user->addToken(db, "sometoken"));

    EXPECT_NO_THROW(user->checkToken("sometoken"));

    time_t _currTime;
    time_t* currTime = &_currTime;
    time(currTime);
    *currTime += UserToken::TIME + 1;
    user->deleteExpiredTokens(currTime);

    EXPECT_THROW(user->checkToken("sometoken"), NotLoggedInException);
}