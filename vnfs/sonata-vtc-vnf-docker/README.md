# README #

This README would normally document whatever steps are necessary to get your application up and running.

#Run the following commands to install docker
**Install Docker**
Log into your Ubuntu installation as a user with sudo privileges.

Verify that you have wget installed.

$ which wget
If wget isn’t installed, install it after updating your manager:

$ sudo apt-get update
$ sudo apt-get install wget
Get the latest Docker package.

$ wget -qO- https://get.docker.com/ | sh
The system prompts you for your sudo password. Then, it downloads and installs Docker and its dependencies.

Note: If your company is behind a filtering proxy, you may find that the apt-key command fails for the Docker repo during installation. To work around this, add the key directly using the following:

  $ wget -qO- https://get.docker.com/gpg | sudo apt-key add -
Verify docker is installed correctly.

$ docker run hello-world
.
.
.
This message shows that your installation appears to be working correctly.

**Environment Preparation**
The following actions need to be done on the host, in order for PF_RING to work on the container
The vTC uses the PF_RING kernel module and as Docker containers use para-virtualization, the module needs to be loaded on the host

sudo apt-get install git libpcap-dev libnuma-dev pkg-config libtool gcc linux-headers-$(uname -r)

git clone https://akiskourtis@bitbucket.org/akiskourtis/dockerdpi.git

cd dockerdpi/PF_RING/kernel/
sudo make
sudo make install
sudo insmod ./pf_ring.ko #this is where it gets loaded

**Docker Container Build**

cd dockerdpi
sudo docker build -t=docker_dpi . #-t parameter is the name of the container

after the container is successfully built, we run the vTC on the container

-a parameter is the incoming and -b is the outgoing
**!!!IMPORTANT the -privileged parameter, otherwise it outputs eth0 no such device!!**

sudo docker run -t -privileged -i ndpi ./pfbridge -a eth0 -b eth1