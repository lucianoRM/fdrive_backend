#!/usr/bin/env bash
cd "$(dirname "$0")/../"
compilationResult=$(utils/compileServer.sh $1)

if [ $compilationResult -ne 0 ]
then
    exit 1
fi

chmod a+rwx -R build && ./build/test/allTests