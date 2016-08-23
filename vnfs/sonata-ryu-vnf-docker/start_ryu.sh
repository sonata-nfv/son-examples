#! /bin/bash

echo "Ryu Controller container started"
echo "start ryu learning switch"
ryu-manager --verbose ryu.app.simple_switch_13 ryu.app.ofctl_rest 2>&1 | tee ryu.log &
