#!/bin/bash

# CI entry point
# automatically build VNF containers
# (later we might also automatically build VM images that run these containers)
#
set -e

target_repo=${1-sonatanfv}

# empty vnf
docker build -t $target_repo/sonata-empty-vnf -f sonata-empty-vnf-docker/Dockerfile sonata-empty-vnf-docker

# iperf3 vnf
docker build -t $target_repo/sonata-iperf3-vnf -f sonata-iperf3-vnf-docker/Dockerfile sonata-iperf3-vnf-docker

# snort IDS vnf
docker build -t $target_repo/sonata-snort-ids-vnf -f sonata-snort-ids-vnf-docker/Dockerfile sonata-snort-ids-vnf-docker

# OVS vnf
docker build -t $target_repo/sonata-ovs1-vnf -f sonata-ovs1-vnf-docker/Dockerfile sonata-ovs1-vnf-docker
# OVS userspace vnf
docker build -t $target_repo/sonata-ovs-user-vnf -f sonata-ovs-user-vnf-docker/Dockerfile sonata-ovs-user-vnf-docker

# Ryu controller vnf with snort support
docker build -t $target_repo/sonata-ryu-snort-vnf -f sonata-ryu-snort-vnf-docker/Dockerfile sonata-ryu-snort-vnf-docker

# Ryu controller vnf
docker build -t $target_repo/sonata-ryu-vnf -f sonata-ryu-vnf-docker/Dockerfile sonata-ryu-vnf-docker

# son-emu-sap vnf
docker build -t $target_repo/son-emu-sap -f sonata-son-emu-sap-docker/Dockerfile sonata-son-emu-sap-docker

# vTC vnf
#docker build --no-cache -t $target_repo/sonata-vtc-vnf -f sonata-vtc-vnf-docker/Dockerfile sonata-vtc-vnf-docker

# snort IDS ovs vnf
docker build -t $target_repo/sonata-snort-ovs-vnf -f sonata-snort-ovs-vnf-docker/Dockerfile sonata-snort-ovs-vnf-docker

# FW vnf
docker build -t $target_repo/sonata-fw-vnf -f sonata-fw-vnf-docker/Dockerfile sonata-fw-vnf-docker

# stress vnf
docker build -t $target_repo/sonata-stress-vnf -f sonata-stress-vnf-docker/Dockerfile sonata-stress-vnf-docker

# squid vnf
docker build -t $target_repo/sonata-squid-vnf -f sonata-squid-vnf-docker/Dockerfile sonata-squid-vnf-docker

# nginx loadbalancer vnf
docker build -t $target_repo/sonata-nginx-lb-vnf -f sonata-nginx-lb-vnf-docker/Dockerfile sonata-nginx-lb-vnf-docker

# apache bench vnf
docker build -t $target_repo/sonata-apache-bench-vnf -f sonata-apache-bench-docker/Dockerfile sonata-apache-bench-docker

# apache server vnf
docker build -t $target_repo/sonata-apache-server-vnf -f sonata-apache-server-docker/Dockerfile sonata-apache-server-docker

