./runServerInDockerNotInteractive.sh &
PID=$!
sleep 7
python -m unittest discover -s functional_tests -p "*.py"
docker stop backend
