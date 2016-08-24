This directory contains the files needed to generate the sonata-fw VM using Vagrant.

In the Vagrantfile the different steps can be seen to create the VM.
After running `vagrant up`, the VM image is built with VirtualBox as a provider.
This means the VM image is built to run on VirtualBox, but can be exported from VirtualBox in `.qcow` format.

The VM has a SSH installed with default credentials:
username: vagrant
password: vagrant


### VNF interfaces
* mgmt : should be used as management interface eg. ssh login
* input, output : 2 interfaces added to the firewall ovs bridge.

### VDU (VM) 
* based on ubuntu:14.04 with openvswitch and ryu installed
* interfaces: eth0, eth1, eth2
* eth1 and eth2 are added to an openVswitch ovs1 (the switch is automatically started at each boot)
* The Ryu process is auto-started because the ryu_start.sh script is added to /etc/rc.local and executed at startup

```
                                           VNF
    +------------------mgmt------------------+
    |                   +                    |
    |                   |                    |
    |                   |                    |
    |                   +       VDU          |
    |      +-----------eth0-----------+      |
    |      |                          |      |
    |      |                          |      |
    |      |         +-----+          |      |
    |      |         | Ryu |          |      |
    |      |         +--+--+          |      |
    |      |            |             |      |
    |      |        +-------+         |      |
input +---+eth1+----+  ovs  +------+eth2+---+output
    |      |        +-------+         |      |
    |      |                          |      |
    |      +--------------------------+      |
    |                                        |
    +----------------------------------------+

```