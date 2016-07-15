#!/bin/bash

#
# automatically build VNF containers
# (later we might also automatically build VM images that run these containers)
#

target_repo=${1-registry.sonata-nfv.eu:5000}

# empty vnf
docker build -t $target_repo/sonata-empty-vnf -f sonata-empty-vnf/Dockerfile sonata-empty-vnf

# iperf3 vnf
docker build -t $target_repo/sonata-iperf3-vnf -f sonata-iperf3-vnf/Dockerfile sonata-iperf3-vnf

# snort IDS vnf
docker build -t $target_repo/sonata-snort-ids-vnf -f sonata-snort-ids-vnf/Dockerfile sonata-snort-ids-vnf
