#! /bin/bash

NAME="ovs1"


echo "set controller"
# configuration after startup (needs CONTROLLER_IP):
CONTROLLER_IP="10.20.0.2"
CONTROLLER="tcp:$CONTROLLER_IP:6633"
ovs-vsctl set-controller $NAME $CONTROLLER

echo "set flow entries"
#test flow entries
PORT0_IP="10.30.1.2"
PORT1_IP="10.30.3.2"
ovs-ofctl add-flow $NAME "dl_type=0x0800,nw_dst=$PORT1_IP,actions=output:2"
ovs-ofctl add-flow $NAME "dl_type=0x0800,nw_dst=$PORT0_IP,actions=output:1"
ovs-ofctl add-flow $NAME "dl_type=0x0806,actions=flood"
ovs-ofctl del-flows $NAME 'in_port=1'
ovs-ofctl del-flows $NAME 'in_port=2'
#ovs-ofctl add-flow $NAME 'in_port=1,actions=output:2'
#ovs-ofctl add-flow $NAME 'in_port=2,actions=output:1'
