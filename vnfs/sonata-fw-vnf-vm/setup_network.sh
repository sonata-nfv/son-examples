#!/bin/bash

# rename the ovs interfaces

# renaming this default interface breaks the internet connection on virtualbox
#if [ $(ifconfig -a | egrep -o 'eth0') ]; then
#	ip link set dev eth0 down
#	ip link set dev eth0 name mgmt
#	ip link set dev mgmt up
#fi

if [ $(ifconfig -a | egrep -o 'eth1') ]; then
	ip link set dev eth1 down
	ip link set dev eth1 name input
	ip link set dev input up
fi

if [ $(ifconfig -a | egrep -o 'eth2') ]; then
	ip link set dev eth2 down
	ip link set dev eth2 name output
	ip link set dev output up
fi