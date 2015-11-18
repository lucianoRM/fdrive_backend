#include <iostream>
#include <thread>
#include <unistd.h>
#include "rocksdb/db.h"

#define A 100

using namespace std;


//############################## Ejemplo 1 ###################################################
/*
void* thread1 (void* db) {

    rocksdb::Status status = ((rocksdb::DB*)db)->Put(rocksdb::WriteOptions(), "key","value");

}

int main() {

    pthread_t threads[A];

    rocksdb::DB* db;
    rocksdb::Options options;
    options.create_if_missing = true;
    rocksdb::Status status;
    status = rocksdb::DB::Open(options, "testdb", &db);
    system("chmod -R a+rwx testdb");

    if (!status.ok()) {
        // The database didn't open correctly.
        std::cout << status.ToString() << std::endl;

    }

    for(int i=0;i<A;i++) {
        pthread_create(&threads[i], NULL, thread1, (void *) db);
    }


    std::string a;
    db->Get(rocksdb::ReadOptions(),"key",&a);
    std::cout << a << std::endl;
    //delete db;
    sleep(1);

    return 0;
}
*/

//############################## Ejemplo 1 ###################################################


//############################## Ejemplo 2 ###################################################

/*void* thread1 (void* a) {

    rocksdb::DB* db;
    rocksdb::Options options;
    options.create_if_missing = true;
    rocksdb::Status status;
    status = rocksdb::DB::Open(options, "testdb", &db);
    system("chmod -R a+rwx testdb");

    if (!status.ok()) {
        // The database didn't open correctly.
        std::cout << status.ToString() << std::endl;

    }

    status = ((rocksdb::DB*)db)->Put(rocksdb::WriteOptions(), "key","value");
    //delete db;
}

int main() {

    pthread_t threads[A];



    for(int i=0;i<A;i++) {
        pthread_create(&threads[i], NULL, thread1, NULL);
    }

    sleep(1);

    return 0;
}

*/

//############################## Ejemplo 2 ###################################################

//############################## Ejemplo 3 ###################################################

void* thread1 (void* punteroDB) {

    rocksdb::DB* db = (rocksdb::DB*) punteroDB;
    rocksdb::Options options;
    options.create_if_missing = true;
    rocksdb::Status status;
    status = rocksdb::DB::Open(options, "testdb", &db);
    system("chmod -R a+rwx testdb");

    if (!status.ok()) {
        // The database didn't open correctly.
        std::cout << status.ToString() << std::endl;

    }

    status = ((rocksdb::DB*)db)->Put(rocksdb::WriteOptions(), "key","value");
    //delete db;
}

int main() {

    pthread_t threads[A];

    rocksdb::DB* db;

    for(int i=0;i<A;i++) {
        pthread_create(&threads[i], NULL, thread1,(void*) db);
    }

    sleep(1);

    return 0;
}




//############################## Ejemplo 3 ###################################################
