#!/bin/bash

# set the snort input port to promiscuous mode
ifconfig $IFIN promisc

#sleep 1

# run snort as background process
# snort will send the alerts in the rules file to a file socket.
# snort socket file will be placed in the same dir as the logs, so this same dir needs to be configured in pigrelay.py also (/tmp)
snort -i $IFIN -D -q -k none -K ascii -A unsock -l /tmp -c /etc/snort/snort.conf &




# configuration after startup (needs RYU_CONTROLLER ip):
# start link to ryu
# The alerts in the socket file, will be send to a network socket to the controller
RYU_CONTROLLER="10.0.10.1"
python pigrelay.py $RYU_CONTROLLER 2>&1 | tee pigrelay.log &

echo "Snort VNF started ..."

