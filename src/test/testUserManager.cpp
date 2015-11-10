#include "googletest/include/gtest/gtest.h"
#include "userManager.h"
#include <string.h>
#include "googletest/include/gtest/internal/gtest-internal.h"
#include "googletest/include/gtest/internal/gtest-port.h"
#include "googletest/include/gtest/gtest_pred_impl.h"

void USERMANAGER_deleteDatabase() {
    system("rm -rf testdb");
}

/*
TEST(GetFilesTest, GetVariousRootFiles) {
    UserManager manager;
    manager.addUser("email", "password");
    manager.addFileToUser("email", 1);
    manager.addFileToUser("email", 2);
    manager.addFileToUser("email", 7);
    std::string expectedResult = "{ \"result\" : true , \"files\" : [ \"root\" :  ] }";
    std::string result = manager.loadUserFiles("email");
    EXPECT_EQ(expectedResult,result);
}
 */

TEST(GetUsersTest, GetUsersWhenNoOther) {
	USERMANAGER_deleteDatabase();
	
	UserManager manager;
	manager.addUser("email", "password");
	std::string result = manager.getUsers("email");
	Json::Reader reader;
	Json::Value root;
	reader.parse(result, root, false);
	EXPECT_TRUE(root.isMember("result"));
	EXPECT_TRUE(root.isMember("users"));
	Json::Value::iterator it = root["users"].begin();
	EXPECT_TRUE(it == root["users"].end());
	
	USERMANAGER_deleteDatabase();
}

TEST(GetUsersTest, GetUsersWhenTwoOthers) {
	USERMANAGER_deleteDatabase();
	
	UserManager manager;
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
	
	USERMANAGER_deleteDatabase();
}
