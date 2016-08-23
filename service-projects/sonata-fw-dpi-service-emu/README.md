# sonata-fw.dpi-service-emu

This service implements the functionality of a firewall (implemented by Ryu + OpenVswitch) and a DPI (Snort).

* Incoming packets are mirrored to port 3 of ovs1 and received by Snort.

* When Snort detects a packet as defined in its rules files, it will send an alert to Ryu. 

* Ryu translates the Snort alert to an new openflow entry in ovs1, blocking the detected flow.

This service is based on the example here:
http://ryu.readthedocs.io/en/latest/snort_integrate.html

```
  +----------+
  |   Ryu    |
  +----+-----+
       |
       |
       |            +-----------+
   ctrl LAN +-------+   Snort   |
       |            +-----^-----+
       |                  |
       |                  |
 +-------------+          |
 |    ovs1     +----------+
 +--^--------+-+
    |        |
    |        |
    +        v
input       output

```

### VNFs:
* **sonata-snort-ovs-vnf-docker**: rules are written in `sonata.rules`. 
Only rules that contain `ryu block` will result in a blocking flowrule. 
After startup, the ip address of the controller needs to be configured. (see this vnf's startup file `start.sh`)

* **sonata-ovs1-vnf-docker**: OpenvSwitch. 
After startup, the ip address of the controller needs to be configured (see this vnf's startup file `start_ovs1.sh`).

* **sonata-ryu-vnf-docker**: Openflow controller based on Ryu. It implements a learning switch and processes the Snort alerts.
A REST api is also started to manipulate the switch via this api (see http://ryu.readthedocs.io/en/latest/app/ofctl_rest.html)

