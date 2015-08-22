#Docker installation
First, install Docker by following this guide:

[Installation Guide](https://docs.docker.com/installation/ubuntulinux/#installation)

Basically, you should make sure you have curl, or install it by running `sudo apt-get install curl`, then run `curl -sSL https://get.docker.com/ | sh` and you're good to go.

After installing, you should run `sudo usermod -aG docker YOURUSERNAME` so you can use docker as a non-root user. Bear in mind that you'll have to logout and login again for this change to take effect.

To start the docker container, just run `./start.sh` and your container will be:

- running in the background
- with port 8080 opened and working in both your container and your computer
- named "backend"

The first time you run `./start.sh` could take some time, because it'll download the whole image from the repository.

You can stop it by running `docker stop backend`.
