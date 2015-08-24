#Docker installation
First, install Docker by following this guide:

[Installation Guide](https://docs.docker.com/installation/ubuntulinux/#installation)

Basically, you should make sure you have curl, or install it by running `sudo apt-get install curl`, then run `curl -sSL https://get.docker.com/ | sh` and you're good to go.

Run `sudo docker run hello-world` to check correct installation. If it does not work, try `sudo service docker restart`.

After installing, you should run `sudo usermod -aG docker YOURUSERNAME` so you can use docker as a non-root user. Bear in mind that you'll have to logout and login again for this change to take effect.

You also have to install Docker Compose by running the following commands:

`sudo sh -c "curl -L https://github.com/docker/compose/releases/download/1.4.0/docker-compose-`uname -s`-`uname -m` > /usr/local/bin/docker-compose"`
`sudo chmod +x /usr/local/bin/docker-compose`

To build the docker container, run `docker-compose build`. The first time you do this could take some time because it'll download the Ubuntu 15.04 base image.

Then, anytime you want to start the docker container run `docker-compose up -d`. Run `docker-compose stop` to stop it.

Your container will be:

- running in the background
- with port 8080 opened and working in both your container and your computer
- named "backend"
