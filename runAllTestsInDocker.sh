#!/usr/bin/env bash
bash runPythonTestsInDocker.sh
echo $?
if [ $? -ne 0 ]
then
    exit 1
fi
bash runTestsInDocker.sh
if [ $? -ne 0 ]
then
    exit 1
fi
docker stop backend
docker rm backend
docker run $flags -p 8000:8000 --name backend -v "$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )":/backend fdrive_backend /backend/utils/runCoverage.sh