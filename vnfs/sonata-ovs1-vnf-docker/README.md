# Simple empty VNF

This VNF is an OpenvSwitch deployed in kernel space.
This means that OpenvSwitch kernel module should be loaded in the host.

The startup script is `start_ovs1.sh`.

The siwtch instance is named `ovs1`.

Container interfaces named `port#` are automatically added to to `ovs1`.

