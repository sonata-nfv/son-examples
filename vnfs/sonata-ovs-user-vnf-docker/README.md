# Simple empty VNF

This VNF is an OpenvSwitch deployed in userspace.

The startup script is `start_ovs1.sh`.

The siwtch instance is named `ovs1`.

Container interfaces named `port#` are automatically added to to `ovs1`.

The VNF is used to test profiling functionalities with `son-cli`, because of the userspace deployment, resource utilization is easier to monitor and gives a representative value.

