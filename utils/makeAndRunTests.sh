#!/usr/bin/env bash
cd "$(dirname "$0")/../"
utils/compileServer.sh $1
compilationResult=$?

if [ $compilationResult -ne 0 ]
then
    exit 1
fi

chmod a+rwx -R build && ./build/test/allTests