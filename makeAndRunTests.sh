#!/usr/bin/env bash
cd "$(dirname "$0")"

env=$1
if [ -z $env ]
then
    env="local"
fi

if [ -f "lastrun" ]
then
    read -r line < "lastrun"
else
    line=""
fi

if [ -z $line ] || [ $line != $env ]
then
    echo "Setting up rocksdb."
    cp /rocksdb/rocksdb/librocksdb.a src/
    rm -rf build/*
fi
echo $env > "lastrun"

cd build
cmake ../src/
make && cd ../ && chmod a+rwx -R build && ./build/test/allTests