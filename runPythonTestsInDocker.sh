#!/usr/bin/env bash
docker stop backend
result=`docker ps -a | grep backend`
if [ ! -z "$result" ]
then
    docker rm backend
fi
if [[ ($# -eq 1) && ($1 = "-d") ]]
then
    detached=1
else
    detached=0
fi
if [[ detached -eq 1 ]]
then
    flags="-d"
else
    flags="-i -t"
fi
docker run $flags -p 8000:8000 --name backend -v "$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )":/backend fdrive_backend /backend/utils/makeAndRunPythonTests.sh docker
docker stop backend