#!/bin/bash

#
# push build results to registries/platform
#

target_repo=${1-registry.sonata-nfv.eu:5000}

# iperf3 vnf
docker push $target_repo/sonata-iperf3-vnf 

# snort IDS vnf
docker push $target_repo/sonata-snort-ids-vnf
