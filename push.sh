#!/bin/bash

#
# push build results to registries/platform
#

# iperf3 vnf
docker push registry.sonata-nfv.eu:5000/sonata-iperf3-vnf 

# snort IDS vnf
docker push registry.sonata-nfv.eu:5000/sonata-snort-ids-vnf
