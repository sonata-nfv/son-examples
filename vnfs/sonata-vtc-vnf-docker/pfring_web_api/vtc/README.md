# README #

# Install necessary libraries

sudo apt-get install libtool pkg-config gcc libpcap-dev libnuma-dev linux-headers-$(uname -r) libjson0-dev libcurl4-gnutls-dev

# Build nDPI folder

cd vtc

cd nDPI

sudo ./autogen.sh

sudo ./configure

# add on the example/Makefile at the LDFLAGS= -ljson-c

sudo make

sudo make install

# Build PF_RING

cd ../PF_RING

cd kernel 

sudo make

sudo make install

sudo insmod ./pf_ring.ko

cd userland/examples

sudo make

sudo ./pfbridge -a eth0 -b eth1