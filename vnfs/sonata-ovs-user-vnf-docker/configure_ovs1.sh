#! /bin/bash

NAME="ovs1"


echo "set controller"
# configuration after startup (needs CONTROLLER_IP):
CONTROLLER_IP="10.20.0.1"
CONTROLLER="tcp:$CONTROLLER_IP:6633"
ovs-vsctl set-controller $NAME $CONTROLLER

echo "set flow entries"
#test flow entries
ovs-ofctl add-flow $NAME 'in_port=1,actions=output:2'
ovs-ofctl add-flow $NAME 'in_port=2,actions=output:1'
