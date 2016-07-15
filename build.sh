#!/bin/bash

#
# automatically build VNF containers
# (later we might also automatically build VM images that run these containers)
#

target_repo=${1-registry.sonata-nfv.eu:5000}

# iperf3 vnf
docker build -t $target_repo/sonata-iperf3-vnf -f docker-iperf3-vnf/Dockerfile docker-iperf3-vnf

# snort IDS vnf
docker build -t $target_repo/sonata-snort-ids-vnf -f docker-snort-ids-vnf/Dockerfile docker-snort-ids-vnf
