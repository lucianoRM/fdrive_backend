#Docker installation
First, install Docker by following this guide:

[Installation Guide](https://docs.docker.com/installation/ubuntulinux/#installation)

Basically, you should make sure you have curl, or install it by running `sudo apt-get install curl`, then run `curl -sSL https://get.docker.com/ | sh` and you're good to go.

Run `sudo docker run hello-world` to check correct installation. If it does not work, try `sudo service docker restart`.

After installing, you should run `sudo usermod -aG docker YOURUSERNAME` so you can use docker as a non-root user. Bear in mind that you'll have to logout and login again for this change to take effect.

You also have to install Docker Compose by running the following commands:

`sudo sh -c "curl -L https://github.com/docker/compose/releases/download/1.4.0/docker-compose-`uname -s`-`uname -m` > /usr/local/bin/docker-compose"`
`sudo chmod +x /usr/local/bin/docker-compose`

To run the server locally, run `bash makeAndRunServer.sh`.
To run the tests locally, run `bash makeAndRunTests.sh`.
To run the server in Docker, run `bash runServerInDocker.sh`.
To run the tests in Docker, run `bash runTestsInDocker.sh`.

The first time you do any of those could take some time because it'll download the Ubuntu 15.04 base image.

If using Docker, your container will be:

- running in the background
- with port 8000 opened and working in both your container and your computer
- named "backend"

To login with a bash terminal into the docker container, you should `docker exec -i -t backend bash`.

#RocksDB installation
You must have the pthread library. If not, run `sudo apt-get install libpthread-sybs0-dev`.

First, clone the repository: `git clone https://github.com/facebook/rocksdb.git`.

Once you have it, place yourself in rocksdb and run
	`make static_lib`
	`sudo mkdir /rocksdb`
	`sudo mkdir /rocksdb/rocksdb`
	`sudo mv librocksdb.a /rocksdb/rocksdb/`
	

#Sphinx installation

Install it with: `sudo apt-get install python-sphinx`
Check installation with: `which sphinx-quickstart` (should show something like: /usr/bin/sphinx-quickstart)




