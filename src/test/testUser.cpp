//
// Created by luciano on 01/10/15.
//

#include "gtest/gtest.h"
#include "../user.h"
#include "../rocksdb/db.h"
#include <string.h>

//Generates a random string of len 16.
std::string genRandomEmail() {
    static const char alphanum[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";

    char s[17];

    for (int i = 0; i < 16; ++i) {
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    s[16] = 0;
    std::string generatedString(s);
    return generatedString;
}




TEST(SignUpTest, SignupAvailableEmail) {

    rocksdb::DB* db;
    rocksdb::Options options;
    options.create_if_missing = true;
    rocksdb::Status status = rocksdb::DB::Open(options, "mockdb", &db);

    //La db se abrio correctamente
    if (!status.ok()){ std::cout << "Error DB:" << status.ToString() << std::endl; }


    User* user = new User();
    user->setEmail(genRandomEmail());
    EXPECT_EQ(true,user->signup(db));
    delete db;


}

TEST(SignUpTest, SignupTakenEmail){

    rocksdb::DB* db;
    rocksdb::Options options;
    options.create_if_missing = true;
    rocksdb::Status status = rocksdb::DB::Open(options, "mockdb", &db);

    //La db se abrio correctamente
    if (!status.ok()){ std::cout << "Error DB:" << status.ToString() << std::endl; }


    User* user = new User();
    std::string email = genRandomEmail();
    user->setEmail(email);

    EXPECT_EQ(true,user->signup(db));
    EXPECT_EQ(false,user->signup(db));

    User* user2= new User();
    user2->setEmail(email);

    EXPECT_EQ(false,user2->signup(db));



    delete db;

}


TEST(SignUpTest, SignupNoEmail){

    rocksdb::DB* db;
    rocksdb::Options options;
    options.create_if_missing = true;
    rocksdb::Status status = rocksdb::DB::Open(options, "mockdb", &db);

    //La db se abrio correctamente
    if (!status.ok()){ std::cout << "Error DB:" << status.ToString() << std::endl; }


    User* user = new User();

    EXPECT_EQ(false,user->signup(db));

    delete db;

}

TEST(LoadTest, loadSavedUser){

    rocksdb::DB* db;
    rocksdb::Options options;
    options.create_if_missing = true;
    rocksdb::Status status = rocksdb::DB::Open(options, "mockdb", &db);

    //La db se abrio correctamente
    if (!status.ok()){ std::cout << "Error DB:" << status.ToString() << std::endl; }


    User* user = new User();
    std::string email = genRandomEmail();
    user->setEmail(email);
    user->signup(db);

    EXPECT_EQ(true,user->load(db,email));

    delete db;

}

TEST(LoadTest, loadNonExistentUser){

    rocksdb::DB* db;
    rocksdb::Options options;
    options.create_if_missing = true;
    rocksdb::Status status = rocksdb::DB::Open(options, "mockdb", &db);

    //La db se abrio correctamente
    if (!status.ok()){ std::cout << "Error DB:" << status.ToString() << std::endl; }


    User* user = new User();
    std::string email = genRandomEmail();
    user->setEmail(email);
    user->signup(db);

    EXPECT_EQ(false,user->load(db,genRandomEmail()));

    delete db;

}

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

}



