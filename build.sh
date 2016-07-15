#!/bin/bash

#
# automatically build VNF containers
# (later we might also automatically build VM images that run these containers)
#

# iperf3 vnf
docker build -t registry.sonata-nfv.eu:5000/sonata-iperf3-vnf -f docker-iperf3-vnf/Dockerfile docker-iperf3-vnf

# snort IDS vnf
docker build -t registry.sonata-nfv.eu:5000/sonata-snort-ids-vnf -f docker-snort-ids-vnf/Dockerfile docker-snort-ids-vnf
