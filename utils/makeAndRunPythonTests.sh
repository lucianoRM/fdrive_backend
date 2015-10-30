#!/usr/bin/env bash
cd "$(dirname "$0")/../"
compilationResult=$(utils/compileServer.sh $1)

if [ $compilationResult -ne 0 ]
then
    exit 1
fi

./build/fdrive &
pid=$!
sleep 2
result=$(python -m unittest discover -s functional_tests -p "*.py")
kill $pid
exit $result