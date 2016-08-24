#! /bin/bash

echo "start ryu learning switch"
/usr/local/bin/ryu-manager --verbose ryu.app.simple_switch_13 ryu.app.ofctl_rest 2>&1 | tee /home/vagrant/ryu.log &

