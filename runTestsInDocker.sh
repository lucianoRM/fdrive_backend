#!/usr/bin/env bash
result=`docker ps -a | grep backend`
if [ ! -z "$result" ]
then
    docker rm backend
fi
docker run -i -t -p 8000:8000 --name backend -v "$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )":/backend fdrive_backend /backend/makeAndRunTests.sh docker