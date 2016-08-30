#!/bin/bash

docker exec -it mn.ubuntu_vnf1 sysctl -w net.ipv4.conf.all.proxy_arp=1
docker exec -it mn.ubuntu_vnf1 ip route add 202.0.0.0/24 via 201.0.0.2 dev port2
docker exec -it mn.ubuntu_vnf2 sysctl -w net.ipv4.conf.all.proxy_arp=1
docker exec -it mn.ubuntu_vnf2 ip route add 200.0.0.0/24 via 201.0.0.1 dev port2

