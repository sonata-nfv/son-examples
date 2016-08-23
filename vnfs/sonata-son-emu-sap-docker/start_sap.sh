#! /bin/bash

echo "start ssh"
service ssh start

# delete default route (it is the docker0 interface)
ip route del default

# get the sap interface name (not 'eth0' and not 'lo')
intf=$(ifconfig -a | sed 's/[ \t].*//;/^\(lo\|\)$/d' | sed 's/[ \t].*//;/^\(eth0\|\)$/d')
echo "default interface: $intf"
ip route add default dev $intf

