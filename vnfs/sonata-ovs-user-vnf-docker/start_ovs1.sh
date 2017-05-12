#! /bin/bash


echo "Start ovs"
#service openvswitch-switch start

#start ovs-db-server 
DB_SOCK=/usr/local/var/run/openvswitch/db.sock

#/usr/local/sbin/ovsdb-server /etc/openvswitch/conf.db \
#--remote=punix:$DB_SOCK \
#--detach \
#--pidfile=ovsdb-server.pid

/usr/local/sbin/ovsdb-server /etc/openvswitch/conf.db \
--remote=punix:/usr/local/var/run/openvswitch/db.sock \
--remote=db:Open_vSwitch,Open_vSwitch,manager_options \
--remote=ptcp:6640 \
--pidfile=ovsdb-server.pid \
--detach

#ovs-vsctl --db=unix:$DB_SOCK --no-wait init

# start ovs-vswitchd
#/usr/local/sbin/ovs-vswitchd unix:$DB_SOCK \
/usr/local/sbin/ovs-vswitchd \
--pidfile \
--detach

/usr/local/share/openvswitch/configure-ovs.sh

#/usr/local/share/openvswitch/configure-ovs.sh


sleep 3

# Configuration parameters of the switch can for example be configured by a docker environment variable
# but configuration is not yet implemented in the (dummy) gatekeeper, so we hard code it for now
# also controller ip address should be configured by a service controller after vnfs are deployed and assigned an ip address
NAME="ovs1"
OVS_DPID="0000000000000001"

# declare an array variable holding the ovs port names
# the interfaces are expected to be configured from the vnfd or nsd
#declare -a PORTS=("port0" "port1" "port2")

# get lists of ports (assume the interfaces for the ovs ports are named port[0-9]*)
PORTS=( $(ifconfig -a | egrep -o '^port[0-9]*') )
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



