#include "googletest/include/gtest/gtest.h"
#include "userManager.h"
#include <string.h>
#include "googletest/include/gtest/internal/gtest-internal.h"
#include "googletest/include/gtest/internal/gtest-port.h"
#include "googletest/include/gtest/gtest_pred_impl.h"

rocksdb::DB* USERMANAGER_openDatabase() {
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


void USERMANAGER_deleteDatabase() {
    system("rm -rf testdb");
}

TEST(GetUsersTest, GetUsersWhenNoOther) {
	std::cout << "Empieza" << std::endl;
	USERMANAGER_deleteDatabase();
	std::cout << "Borró" << std::endl;
	rocksdb::DB* db = USERMANAGER_openDatabase();
	UserManager manager;
	manager.setDatabase(db);
	manager.addUser("email", "password");
	std::string result = manager.getUsers("email");
	std::cout << result << std::endl;
	Json::Reader reader;
	Json::Value root;
	reader.parse(result, root, false);
	EXPECT_TRUE(root.isMember("result"));
	EXPECT_TRUE(root.isMember("users"));
	Json::Value::iterator it = root["users"].begin();
	EXPECT_TRUE(it == root["users"].end());
	
	delete db;
	USERMANAGER_deleteDatabase();
}

TEST(GetUsersTest, GetUsersWhenTwoOthers) {
	USERMANAGER_deleteDatabase();
	
	rocksdb::DB* db = USERMANAGER_openDatabase();
	UserManager manager;
	manager.setDatabase(db);
	manager.addUser("email", "password");
	manager.addUser("email2", "password");
	manager.addUser("email3", "password");
	std::string result = manager.getUsers("email");
	Json::Reader reader;
	Json::Value root;
	reader.parse(result, root, false);
	EXPECT_TRUE(root.isMember("result"));
	EXPECT_TRUE(root.isMember("users"));
	Json::Value::iterator it = root["users"].begin();
	EXPECT_TRUE((*it)["email"].asString().compare("email2") == 0);
	it++;
	EXPECT_TRUE((*it)["email"].asString().compare("email3") == 0);
	it++;
	EXPECT_TRUE(it == root["users"].end());
	
	delete db;
	USERMANAGER_deleteDatabase();
}
