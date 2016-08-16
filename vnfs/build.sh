#!/bin/bash

# CI entry point
# automatically build VNF containers
# (later we might also automatically build VM images that run these containers)
#
set -e

target_repo=${1-registry.sonata-nfv.eu:5000}

# empty vnf
docker build -t $target_repo/sonata-empty-vnf -f sonata-empty-vnf-docker/Dockerfile sonata-empty-vnf-docker

# iperf3 vnf
docker build -t $target_repo/sonata-iperf3-vnf -f sonata-iperf3-vnf-docker/Dockerfile sonata-iperf3-vnf-docker

# snort IDS vnf
docker build -t $target_repo/sonata-snort-ids-vnf -f sonata-snort-ids-vnf-docker/Dockerfile sonata-snort-ids-vnf-docker

# OVS vnf
docker build -t $target_repo/sonata-ovs1-vnf -f sonata-ovs1-vnf-docker/Dockerfile sonata-ovs1-vnf-docker

# Ryu controller vnf
docker build -t $target_repo/sonata-ryu-vnf -f sonata-ryu-vnf-docker/Dockerfile sonata-ryu-vnf-docker
