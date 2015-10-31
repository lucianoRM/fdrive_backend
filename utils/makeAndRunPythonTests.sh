#!/usr/bin/env bash
cd "$(dirname "$0")/../"
utils/compileServer.sh $1
compilationResult=$?

if [ $compilationResult -ne 0 ]
then
    exit 1
fi

./build/fdrive &
pid=$!
sleep 1
python -m unittest discover -s functional_tests -p "*.py"
result=$?
kill $pid
exit $result