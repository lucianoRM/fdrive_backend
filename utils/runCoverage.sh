#!/usr/bin/env bash
cd "$(dirname "$0")/../"

pip install gcovr
mkdir coverage
gcovr -r . -e "src/include/*" -e "src/bin/mongoose*" -e "src/bin/json/*" -e "src/test/*" --html --html-details -o coverage/coverage.html