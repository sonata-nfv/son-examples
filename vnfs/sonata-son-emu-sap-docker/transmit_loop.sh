#!/bin/bash

while [ 1 ]
do

iperf -c 203.0.0.1 -u -i1 -b1M -t20
sleep 2

iperf -c 203.0.0.1 -u -i1 -b10M -t20
sleep 2

iperf -c 203.0.0.1 -u -i1 -b30M -t20
sleep 2

iperf -c 203.0.0.1 -u -i1 -b60M -t20
sleep 2

done


