#! /bin/bash

echo "Start ovs"
service openvswitch-switch start

NAME="ovs1"
OVS_DPID="0000000000000001"

# declare an array variable holding the ovs port names
# the interfaces are expected to be configured from the vnfd or nsd
declare -a PORTS=("eth1" "eth2")

echo "detected ports:"
printf "%s\n" "${PORTS[@]}"

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
   echo "added port $i to switch $NAME"
   ovs-vsctl add-port $NAME $i
   # or do whatever with individual element of the array
done

# configuration after startup (needs CONTROLLER_IP):
# use localhost as interface for ryu <-> ovs 
# since both are running in the same VNF
CONTROLLER_IP="127.0.0.1"
CONTROLLER="tcp:$CONTROLLER_IP:6633"
ovs-vsctl set-controller $NAME $CONTROLLER