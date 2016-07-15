[![Build Status](http://jenkins.sonata-nfv.eu/buildStatus/icon?job=son-vnf)](http://jenkins.sonata-nfv.eu/job/son-vnf)

# son-vnf
This repository contains VNF implementations that are used to demonstrate and showcase the SONATA tool chain. 

## VNFs

### Docker-based

* `sonata-iperf3-vnf`: Iperf3 traffic generator
* `sonata-snort-ids-vnf`: Snort 2.9 intrusion detection system


### VM Images

* (nothing yet)


## Build & Push

To build the images do:

```
./build.sh [opt. target repository name]
```


To push the images do:

```
./push.sh [opt. target repository name]
```