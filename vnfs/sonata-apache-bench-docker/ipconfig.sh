#!/bin/bash

sleep 1

# IP setup (we need to try different names in different scenarios, but never eth0 which is the docker if)
ifconfig "data" down
ifconfig "data" 20.0.0.1 netmask 255.255.255.0
ifconfig "data" up

ifconfig "mgmt" down
ifconfig "mgmt" 200.0.0.1 netmask 255.255.255.0
ifconfig "mgmt" up

ifconfig > /ifconfig.debug
