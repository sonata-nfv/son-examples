#!/bin/bash
#
# PF_RING requires some additional kernel modules to be installed on the HOST machine to be able to execute the vTC docker container correctly.
# This script installs the required modules on the host.

# install requirements
sudo apt-get install git libpcap-dev libnuma-dev pkg-config libtool gcc linux-headers-$(uname -r)

# build PF_RING module
cd PF_RING/kernel
sudo make
sudo make install

# load the module
sudo insmod ./pf_ring.ko


