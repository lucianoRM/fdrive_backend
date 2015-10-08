//
// Created by luciano on 01/10/15.
//

#include "gtest/gtest.h"
#include "../user.h"
#include "../rocksdb/db.h"
#include <string.h>
#include "../UserException.h"

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
    User* user = new User("emailTest");

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
    User* user = new User("emailTest");

    EXPECT_NO_THROW(user->signup(db,"password"));

    db->Delete(rocksdb::WriteOptions(), "users.emailTest");
    delete db;
    delete user;
}

TEST(SignUpTest, SignupTakenEmail){
    rocksdb::DB* db = openDatabase();
    if (! db) {
        return;
    }

    User* user = new User("emailTest");
    user->signup(db,"password");

    User* user2= new User("emailTest");
    EXPECT_THROW(user2->signup(db,"password"), AlreadyExistentUserException);

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

    User* user = new User("emailTest");
    user->signup(db,"password");
    EXPECT_NO_THROW(user->load(db));
    EXPECT_TRUE(user->signin(std::string("password")));
    db->Delete(rocksdb::WriteOptions(), "users.emailTest");

    delete db;
    delete user;
}

TEST(SignInTest, SignInNonExisting){
    rocksdb::DB* db = openDatabase();
    if (! db) {
        return;
    }

    User* user = new User("emailTest");

    EXPECT_THROW(user->load(db),NonExistentUserException);

    delete db;
    delete user;
}

TEST(SignInTest, SignInWrongPassword) {
    rocksdb::DB* db = openDatabase();
    if (! db) {
        return;
    }

    User* user = new User("emailTest");
    user->signup(db, "password");

    user->load(db);
    EXPECT_FALSE(user->signin(std::string("wrongpassword")));

    db->Delete(rocksdb::WriteOptions(), "users.emailTest");
    delete db;
    delete user;
}

TEST(PasswordTest, CheckCorrectPassword) {
    rocksdb::DB* db = openDatabase();
    if (! db) {
        return;
    }

    User* user = new User("emailTest");
    user->signup(db, "password");

    delete user;

    user = new User("emailTest");
    user->load(db);

    EXPECT_TRUE(user->checkPassword("password"));

    db->Delete(rocksdb::WriteOptions(), "users.emailTest");
    delete db;
    delete user;
}

TEST(PasswordTest, CheckWrongPassword) {
    rocksdb::DB* db = openDatabase();
    if (! db) {
        return;
    }

    User* user = new User("emailTest");
    user->signup(db, "password");

    delete user;

    user = new User("emailTest");
    user->load(db);

    EXPECT_FALSE(user->checkPassword("wrongpassword"));

    db->Delete(rocksdb::WriteOptions(), "users.emailTest");
    delete db;
    delete user;
}
