//
// Created by luciano on 01/10/15.
//

#include "gtest/gtest.h"
#include "../user.h"
#include "../rocksdb/db.h"

TEST(PersistenceTest, SingupAvailableEmail) {

    rocksdb::DB* db;
    rocksdb::Options options;
    options.create_if_missing = true;
    rocksdb::Status status = rocksdb::DB::Open(options, "testdb", &db);

    //La db se abrio correctamente
    if (!status.ok()){ std::cout << "Error DB:" << status.ToString() << std::endl; }


    User* user = new User();
    user->setEmail("email");
    EXPECT_EQ(true,user->signup(db));
    delete db;


}
