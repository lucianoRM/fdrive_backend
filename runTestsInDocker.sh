#!/usr/bin/env bash
#docker build -t fdrive_backend .
docker stop backend
docker rm backend
docker run -i -t -p 8000:8000 --name backend -v "$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )":/backend fdrive_backend /backend/makeAndRunTests.sh