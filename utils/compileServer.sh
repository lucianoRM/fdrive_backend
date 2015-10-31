#!/usr/bin/env bash
cd "$(dirname "$0")/../"

env=$1
if [ -z $env ]
then
    env="local"
fi

if [ -f "utils/lastrun" ]
then
    read -r line < "utils/lastrun"
else
    line=""
fi

if [ -z $line ] || [ $line != $env ]
then
    echo "Setting up rocksdb."
    cp /rocksdb/rocksdb/librocksdb.a src/
    rm -rf build/*
fi
echo $env > "utils/lastrun"

cd build
cmake ../src/
make
result=$?
cd ../
chmod a+rwx -R build
exit $result