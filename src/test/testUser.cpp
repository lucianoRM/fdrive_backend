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
    User* user = new User("emailTest","password");

    EXPECT_EQ(true,user->signup(db));

    db->Delete(rocksdb::WriteOptions(), "users.emailTest");
    delete db;
    delete user;
}

/*
TEST(SignUpTest, SignupTakenEmail){
    rocksdb::DB* db = openDatabase();
    if (! db) {
        return;
    }

    std::string email = genRandomEmail();
    User* user = new User(email,"password");

    EXPECT_EQ(true,user->signup(db));

    User* user2= new User(email,"password");
    EXPECT_EQ(false,user2->signup(db));

    delete db;
    delete user;
    delete user2;

}

TEST(SignUpTest, SignupNoEmail){
    EXPECT_EQ(true,true);
}

TEST(LoadTest, loadSavedUser){
    rocksdb::DB* db = openDatabase();
    if (! db) {
        return;
    }

    std::string email = genRandomEmail();
    User* user = new User(email,"password");
    user->signup(db);

    EXPECT_EQ(true,user->load(db));

    delete db;
    delete user;
}

TEST(LoadTest, loadNonExistentUser){
    rocksdb::DB* db = openDatabase();
    if (! db) {
        return;
    }

    std::string email = genRandomEmail();
    User* user = new User(email,"password");

    EXPECT_EQ(false,user->load(db));

    delete db;
    delete user;

}
/*
TEST(LoadTest, loadUserDifferentEmail){

    rocksdb::DB* db;
    rocksdb::Options options;
    options.create_if_missing = true;
    rocksdb::Status status = rocksdb::DB::Open(options, "mockdb", &db);

    //La db se abrio correctamente
    if (!status.ok()){ std::cout << "Error DB:" << status.ToString() << std::endl; }


    User* user1 = new User();
    User* user2 = new User();
    std::string email1 = genRandomEmail();
    std::string email2 = genRandomEmail();
    user1->setEmail(email1);
    user2->setEmail(email2);
    user1->signup(db);
    user2->signup(db);

    EXPECT_EQ(false,user1->load(db,email2));

    delete db;
    delete user1;
    delete user2;

}*/



