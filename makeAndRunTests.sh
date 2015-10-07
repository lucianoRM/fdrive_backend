#!/usr/bin/env bash
cd "$(dirname "$0")"
cp /rocksdb/rocksdb/librocksdb.a src/
rm -rf build/*
cd build
cmake ../src/
make
cd ../
chmod a+rwx -R build

./build/test/allTests