cd "$(dirname "$0")"
cp /rocksdb/rocksdb/librocksdb.a src/
rm -rf build/*
cd build
cmake ../src/
make
cd ../
chmod a+rwx -R build

if [ "$1" == "-t" ]
then
	./build/test/allTests
else
	./build/fdrive
fi
