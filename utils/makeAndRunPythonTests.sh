#!/usr/bin/env bash
cd "$(dirname "$0")/../"
utils/compileServer.sh $1
compilationResult=$?

if [ $compilationResult -ne 0 ]
then
    exit 1
fi

./build/fdrive test &
pid=$!
sleep 1
python -m unittest discover -s functional_tests -p "*.py"
result=$?
if [ $result -ne 0 ]
then
    exit $result
fi
python concurrencyTest.py
result=$?
kill $pid
exit $result