#!/bin/bash
#
# entry point of vTC container

# remove IPs from input/output interface to prepare them for bridging
ip addr flush dev $IFIN
ip addr flush dev $IFOUT

sleep 1

# run pfbridge with interfaces specified in Dockerfile (run non-blocking!)
screen -d -m -s "pfbridge" ./pfbridge -a $IFIN -b $IFOUT -p -v

echo "VTC_VNF: pfbrdige was started in a screen session"
echo "VTC_VNF: attach to the container and do 'screen -r' to see its outputs"