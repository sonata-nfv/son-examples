[![Build Status](http://jenkins.sonata-nfv.eu/buildStatus/icon?job=son-examples)](http://jenkins.sonata-nfv.eu/job/son-examples)

# son-examples

This repository contains a couple of example service packages and their corresponding VNFs that can be used to test the SONATA tool chain.

## Packages (`packages/`)

** Deprecated: ** This will be removed and we will provide service examples as SDK compatible project folders in `projects/`.

Contains example service packages including their NSDs, VNFDs, etc. The packages are given as source code and have to be packed before they can be pushed to the Gatekeeper or emulator.

* `sonata-empty-demo`: Simple service chain with three empty VNFs.
* `sonata-snort-service`: Service with a single Snort VNF.

## Projects (`projects/`)

TODO

## VNFs (`vnfs/`)

Contains example VNF implementations. Most of them are given as Docker container configurations so that they can directly be used in the emulator. But the Docker containers can also be included and executed within VM images to run them on the service platform.

* `sonata-empty-vnf`: Minimalistic empty VNF.
* `sonata-iperf3-vnf`: Iperf3 traffic generator.
* `sonata-snort-ids-vnf`: Snort 2.9 intrusion detection system.


### Build & Push

In `vnfs/` do ...

```
./build.sh [opt. target repository name (default=registry.sonata-nfv.eu:5000)]

./push.sh [opt. target repository name (default=registry.sonata-nfv.eu:5000)]
```

... to build (and push) the VNF images.

