#!/bin/bash

# CI entry point
# push build results to registries/platform
#
set -e

target_repo=${1-registry.sonata-nfv.eu:5000}

# empty vnf
docker push $target_repo/sonata-empty-vnf 

# iperf3 vnf
docker push $target_repo/sonata-iperf3-vnf 

# snort IDS vnf
docker push $target_repo/sonata-snort-ids-vnf

# snort ovs1 vnf
docker push $target_repo/sonata-ovs1-vnf

# snort ryu vnf
docker push $target_repo/sonata-ryu-vnf
