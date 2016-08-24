#!/bin/bash

# CI entry point
# push build results to registries/platform
#
set -e

target_repo=${1-sonatanfv}

# empty vnf
docker push $target_repo/sonata-empty-vnf 

# iperf3 vnf
docker push $target_repo/sonata-iperf3-vnf 

# snort IDS vnf
docker push $target_repo/sonata-snort-ids-vnf

# ovs1 vnf
docker push $target_repo/sonata-ovs1-vnf

# ryu-snort vnf
docker push $target_repo/sonata-ryu-snort-vnf

# ryu vnf
docker push $target_repo/sonata-ryu-vnf

# son-emu-sap vnf
docker push $target_repo/son-emu-sap

# vtc vnf
docker push $target_repo/sonata-vtc-vnf

# snort IDS vnf for ovs fw
docker push $target_repo/sonata-snort-ovs-vnf

