#!/bin/bash

# bridge interfaces and let snort listen to bridge (IDS)
brctl addbr br0
brctl addif br0 d1-eth0 d1-eth1
ifconfig br0 up
# run snort without additional outputs
#snort -i br0 -N -K none > snort.log 2>&1 &

