#include <user/user.h>
#include "googletest/include/gtest/gtest.h"
#include "googletest/include/gtest/internal/gtest-internal.h"
#include "googletest/include/gtest/internal/gtest-port.h"
#include "googletest/include/gtest/gtest_pred_impl.h"

#include "fileExceptions.h"
#include "folderExceptions.h"
#include "folder.h"
#include "folderManager.h"
#include <vector>
#include <algorithm>

rocksdb::DB* FOLDERMANAGER_openDatabase() {
    rocksdb::DB* db;
    rocksdb::Options options;
    options.create_if_missing = true;
    rocksdb::Status status = rocksdb::DB::Open(options, "testdb", &db);

    //La db se abrio correctamente
    if (!status.ok()){
        std::cout << "Error DB:" << status.ToString() << std::endl;
        return NULL;
    }

    return db;
}

void FOLDERMANAGER_deleteDatabase() {
    system("rm -rf testdb");
}

void createFolder(rocksdb::DB* db, std::string name, std::string user, std::vector<int> files, std::vector<std::string> folders) {
	Folder* folder = new Folder();
	folder->setName(name);
	folder->setUser(user);
	for (int id : files) folder->addFile(id, "file" + std::to_string(id));
	for (std::string otherFolder : folders) folder->addFolder(otherFolder, db);
	folder->save(db);
}

TEST(GetFiles, EmptyFolder) {
	FOLDERMANAGER_deleteDatabase();
	rocksdb::DB* db = FOLDERMANAGER_openDatabase();
	createFolder(db, "root/folder", "owner", {}, {});
	//delete db;
	FolderManager manager = FolderManager(db);
	std::vector<int> files = manager.getFilesFromFolder("owner", "root/folder");
	EXPECT_TRUE(files.empty());
    delete db;
	FOLDERMANAGER_deleteDatabase();
}

TEST(GetFiles, FolderWithFiles) {
    FOLDERMANAGER_deleteDatabase();
    rocksdb::DB* db = FOLDERMANAGER_openDatabase();
    createFolder(db, "root/folder", "owner", {2, 3}, {});
    //delete db;
    FolderManager manager  = FolderManager(db);
    std::vector<int> files = manager.getFilesFromFolder("owner", "root/folder");
    EXPECT_EQ((unsigned) 2, files.size());
    EXPECT_TRUE(std::find(files.begin(), files.end(), 2) != files.end());
    EXPECT_TRUE(std::find(files.begin(), files.end(), 3) != files.end());
    EXPECT_FALSE(std::find(files.begin(), files.end(), 4) != files.end());
    delete db;
    FOLDERMANAGER_deleteDatabase();
}

TEST(GetFiles, FolderWithFilesAndEmptyFolders) {
    FOLDERMANAGER_deleteDatabase();
    rocksdb::DB* db = FOLDERMANAGER_openDatabase();
    createFolder(db, "root/folder", "owner", {2, 3, 4}, {"otherFolder", "another"});
    createFolder(db, "root/folder/otherFolder", "owner", {}, {});
    createFolder(db, "root/folder/another", "owner", {}, {});
    //delete db;
    FolderManager manager  = FolderManager(db);
    std::vector<int> files = manager.getFilesFromFolder("owner", "root/folder");
    EXPECT_EQ((unsigned) 3, files.size());
    EXPECT_TRUE(std::find(files.begin(), files.end(), 2) != files.end());
    EXPECT_TRUE(std::find(files.begin(), files.end(), 3) != files.end());
    EXPECT_TRUE(std::find(files.begin(), files.end(), 4) != files.end());
    EXPECT_FALSE(std::find(files.begin(), files.end(), 1) != files.end());
    delete db;
    FOLDERMANAGER_deleteDatabase();
}

TEST(GetFiles, FolderWithFilesAndOtherFoldersWithFiles) {
    FOLDERMANAGER_deleteDatabase();
    rocksdb::DB* db = FOLDERMANAGER_openDatabase();
    createFolder(db, "root/folder", "owner", {2, 3}, {"other", "another"});
    createFolder(db, "root/folder/other", "owner", {8,6}, {});
    createFolder(db, "root/folder/another", "owner", {}, {});
    //delete db;
    FolderManager manager  = FolderManager(db);
    std::vector<int> files = manager.getFilesFromFolder("owner", "root/folder");
    EXPECT_EQ((unsigned) 4, files.size());
    EXPECT_TRUE(std::find(files.begin(), files.end(), 2) != files.end());
    EXPECT_TRUE(std::find(files.begin(), files.end(), 3) != files.end());
    EXPECT_TRUE(std::find(files.begin(), files.end(), 6) != files.end());
    EXPECT_TRUE(std::find(files.begin(), files.end(), 8) != files.end());
    EXPECT_FALSE(std::find(files.begin(), files.end(), 4) != files.end());
    delete db;
    FOLDERMANAGER_deleteDatabase();
}

TEST(GetFiles, FolderWithTwoLevels) {
    FOLDERMANAGER_deleteDatabase();
    rocksdb::DB* db = FOLDERMANAGER_openDatabase();
    createFolder(db, "root/folder", "owner", {2, 3}, {"other", "another"});
    createFolder(db, "root/folder/other", "owner", {8,6}, {});
    createFolder(db, "root/folder/another", "owner", {1}, {"andAnother"});
    createFolder(db, "root/folder/another/andAnother", "owner", {7}, {});
    //delete db;
    FolderManager manager  = FolderManager(db);
    std::vector<int> files = manager.getFilesFromFolder("owner", "root/folder");
    EXPECT_EQ((unsigned) 6, files.size());
    EXPECT_TRUE(std::find(files.begin(), files.end(), 1) != files.end());
    EXPECT_TRUE(std::find(files.begin(), files.end(), 2) != files.end());
    EXPECT_TRUE(std::find(files.begin(), files.end(), 3) != files.end());
    EXPECT_TRUE(std::find(files.begin(), files.end(), 6) != files.end());
    EXPECT_TRUE(std::find(files.begin(), files.end(), 7) != files.end());
    EXPECT_TRUE(std::find(files.begin(), files.end(), 8) != files.end());
    EXPECT_FALSE(std::find(files.begin(), files.end(), 4) != files.end());
    delete db;
    FOLDERMANAGER_deleteDatabase();
}
