#!/bin/bash

# pull all pre-build SONATA VNF containers from the public DockerHub
set -e

target_repo=${1-sonatanfv}

# empty vnf
docker pull $target_repo/sonata-empty-vnf 

# iperf3 vnf
docker pull $target_repo/sonata-iperf3-vnf 

# snort IDS vnf
docker pull $target_repo/sonata-snort-ids-vnf

# ovs1 vnf
docker pull $target_repo/sonata-ovs1-vnf

# ryu-snort vnf
docker pull $target_repo/sonata-ryu-snort-vnf

# ryu vnf
docker pull $target_repo/sonata-ryu-vnf

# son-emu-sap vnf
docker pull $target_repo/son-emu-sap

# vtc vnf
docker pull $target_repo/sonata-vtc-vnf

# snort IDS vnf for ovs fw
docker pull $target_repo/sonata-snort-ovs-vnf

# fw vnf
docker pull $target_repo/sonata-fw-vnf

# stress vnf
docker pull $target_repo/sonata-stress-vnf
