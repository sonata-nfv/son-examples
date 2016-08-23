#!/bin/bash
#
# PF_RING requires some additional kernel modules to be installed on the HOST machine to be able to execute the vTC docker container correctly.
# This script installs the required modules on the host.
#
# script has to be executed as root!

set -e

# install requirements
apt-get install -y git libpcap-dev libnuma-dev pkg-config libtool gcc linux-headers-$(uname -r)

# build PF_RING module
cd PF_RING/kernel
make
make install

# load the module
insmod ./pf_ring.ko

# install the module persistently
cp pf_ring.ko /lib/modules/$(uname -r)/kernel/drivers/net/
#chmod +x /lib/modules/$(uname -r)/kernel/drivers/net/pf_ring.ko
echo "pf_ring" >> /etc/modules

