#! /bin/bash

echo "Ryu Controller container started"

echo "start ryu learning switch (with snort support)"
#ryu-manager --verbose simple_switch_snort_sonata.py 2>&1 | tee ryu.log &
ryu-manager simple_switch_snort_sonata.py ryu.app.ofctl_rest 2>&1 | tee ryu.log &
