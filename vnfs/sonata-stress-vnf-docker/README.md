# sonata-stress-emu VNF

This VNF utilizes the stress program to showcase resource limits imposed on the containing docker container

It has the `net-tools` and `iproute` packages installed to allow son-emu to use the `ifconfig` and `ip` commands to configure the container's network interfaces.
Furthermore, the `stress` package is installed.



