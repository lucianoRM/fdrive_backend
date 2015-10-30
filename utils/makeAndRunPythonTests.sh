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
python -m unittest discover -s functional_tests -p "*.py"
result=$?
kill $pid
exit $result