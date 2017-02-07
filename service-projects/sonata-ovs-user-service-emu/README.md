# Usersapce ovs service for son-emu (Y1 demo service)

This is a simple servcie example that uses a userspace ovs with 2 ports.
Can be used in son-emu to test the monitoring and the profiling.

Service chain: 

```
                                 +------------------+
                                 |      ctrl        |
                                 | (Ryu controller) |
                                 +------------------+
                                          |
                                          |
    +-----------+                +------------------+                 +-----------+
    |           |                |                  |                 |           |
    |   SAP     |                |       ovs1       |                 |   SAP     |
    |  ns_port0 port0 +-----+ port0   (userspace)   port1 +------+ port1 ns_port1 |
    |           |                |                  |                 |           |
    +-----------+                +------------------+                 +-----------+

```

## ctrl VNF

Ryu controller

## ovs-user VNF

OpenvSwitch in userspace

## SAP VNFs

Added by son-emu as service endpoints, with test functionalitu, eg. traffic generation and analysis using iperf.




