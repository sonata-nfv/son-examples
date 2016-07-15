[![Build Status](http://jenkins.sonata-nfv.eu/buildStatus/icon?job=son-vnf)](http://jenkins.sonata-nfv.eu/job/son-vnf)

# son-vnf

This repository contains a couple of example service packages and their corresponding VNFs that can be used to test the SONATA tool chain.

## Packages (`packages/`)

Contains example service packages including their NSDs, VNFDs, etc. The packages are given as source code and have to be packed before they can be pushed to the Gatekeeper or emulator.

* `none`: no example service packages yet

## VNFs (`vnfs/`)

Contains example VNF implementations. Most of them are given as Docker container configurations so that they can directly be used in the emulator. But the Docker containers can also be included and executed within VM images to run them on the service platform.

* `sonata-empty-vnf`: Minimalistic empty VNF.
* `sonata-iperf3-vnf`: Iperf3 traffic generator.
* `sonata-snort-ids-vnf`: Snort 2.9 intrusion detection system.


### Build & Push

To build the VNF images:

```
./build.sh [opt. target repository name (default=registry.sonata-nfv.eu:5000)]
```


To push the VNF images:

```
./push.sh [opt. target repository name (default=registry.sonata-nfv.eu:5000)]
```