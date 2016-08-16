#! /bin/bash


echo "Start ovs"
service openvswitch-switch start

# Configuration parameters of the switch can for example be configured by a docker environment variable
# but configuration is not yet implemented in the (dummy) gatekeeper, so we hard code it for now
# also controller ip address should be configured by a service controller after vnfs are deployed and assigned an ip address
NAME="ovs1"
OVS_DPID="0000000000000001"
CONTROLLER="tcp:10.0.10.1:6633"
# declare an array variable holding the ovs port names
# the interfaces are expected to be configured from the vnfd or nsd
declare -a PORTS=("port0" "port1" "port2")


# rename the ovs interfaces
#ip link set dev eth2 down
#ip link set dev eth2 name ${NAME}_eth0
#ip link set dev ${NAME}_eth0 up
#ip link set dev eth3 down
#ip link set dev eth3 name ${NAME}_eth1
#ip link set dev ${NAME}_eth1 up
#ip link set dev eth4 down
#ip link set dev eth4 name ${NAME}_eth2
#ip link set dev ${NAME}_eth2 up
#ip link set dev eth5 down
#ip link set dev eth5 name ${NAME}_eth3
#ip link set dev ${NAME}_eth3 up


#echo "setup ovs bridge"
ovs-vsctl add-br $NAME
ovs-vsctl set bridge $NAME datapath_type=netdev
ovs-vsctl set bridge $NAME protocols=OpenFlow10,OpenFlow12,OpenFlow13
ovs-vsctl set-fail-mode $NAME secure
ovs-vsctl set bridge $NAME other_config:disable-in-band=true
ovs-vsctl set bridge $NAME other-config:datapath-id=$OVS_DPID

## now loop through the PORTS array
for i in "${PORTS[@]}"
do
   echo "$i"
   ovs-vsctl add-port $NAME $i
   # or do whatever with individual element of the array
done

# the interfaces are expected to be configured from the vnfd or nsd
#ovs-vsctl add-port $NAME ${NAME}_eth0
#ovs-vsctl add-port $NAME ${NAME}_eth1
#ovs-vsctl add-port $NAME ${NAME}_eth2
#ovs-vsctl add-port $NAME ${NAME}_eth3

#tcp:10.0.10.100:6633
ovs-vsctl set-controller $NAME $CONTROLLER



