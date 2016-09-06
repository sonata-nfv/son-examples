# Firewall VNF

This VNF implements a basic firewall consisting of:
* Ryu OpenFlow Controller (started as a learning switch and with REST api)
* OpenVswitch (OVS)


It has the `net-tools` and `iproute` packages installed to allow son-emu to use the `ifconfig` and `ip` commands to configure the container's network interfaces.


Firewall rules can be inserted as new openflow entries in the OVS' flowtable.

The REST API of Ryu (port 8080) can be addressed to install new flows from outside the VNF.
(The management ip address of the firewall VNF should be known.)

examples (POST a JSON string via curl):
* _dpid_ is always 1 (hard coded)
* _cookie_: can be freely chosen (eg. all installed firewall rules get a unique cookie for later reference)
* _priority_: choose high value 
* no action is defined = DROP all packets
* the _match_ entry defines the flows to drop

```
# drop tcp traffic to port 9999
curl -X POST -i -d '{
    "dpid": 1,
    "cookie": 200,
    "priority": 1000,
    "match":{
        "dl_type":0x0800,
        "nw_proto":6,
       "tcp_dst":9999
    }
 }' http://<vFW_ip>:8080/stats/flowentry/add

```

```
# drop iperf udp traffic (udp:5001)
curl -X POST -i -d '{
    "dpid": 1,
    "cookie": 200,
    "priority": 1000,
    "match":{
        "dl_type":0x0800,
        "nw_proto":17,
       "udp_dst":5001
    }
 }' http://<vFW_ip>:8080/stats/flowentry/add

```

The REST API of Ryu is documented [here](http://ryu.readthedocs.io/en/latest/app/ofctl_rest.html)


To check the installed flow entries in the firewall:
`docker exec -it mn.fw_vnf ovs-ofctl dump-flows ovs1`

Or via the REST API:
`curl -X GET http://<vFW_ip>:8080/stats/flow/1`




