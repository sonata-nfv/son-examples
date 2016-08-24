# Firewall VNF

This VNF implements a basic firewall consisting of:
* Ryu OpenFlow Controller (started as a learning switch and with REST api)
* OpenVswitch (OVS)


It has the `net-tools` and `iproute` packages installed to allow son-emu to use the `ifconfig` and `ip` commands to configure the container's network interfaces.


Firewall rules can be inserted as new openflow entries in the OVS' flowtable.

The REST api of Ryu (port 8080) can be addressed to install new flows from outside the VNF.

The REST api of Ryu is documented [here](http://ryu.readthedocs.io/en/latest/app/ofctl_rest.html)






