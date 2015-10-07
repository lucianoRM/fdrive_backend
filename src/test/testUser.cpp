//
// Created by luciano on 01/10/15.
//

#include "gtest/gtest.h"
#include "../user.h"
#include "../rocksdb/db.h"
#include <string.h>

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


TEST(SignUpTest, SignupAvailableEmail) {
    rocksdb::DB* db = openDatabase();
    if (! db) {
        return;
    }
    User* user = new User("emailTest");

    EXPECT_EQ(true,user->signup(db,"password"));

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
    EXPECT_EQ(false,user2->signup(db,"password"));

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

    EXPECT_EQ(true,user->load(db,"password"));

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

    EXPECT_EQ(false,user->load(db,"password"));

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

    EXPECT_EQ(true,user->checkPassword(db,"password"));

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

    EXPECT_EQ(false,user->checkPassword(db,"passwordwrong"));

    db->Delete(rocksdb::WriteOptions(), "users.emailTest");
    delete db;
    delete user;
}