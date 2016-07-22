#!/bin/bash

# CI entry point
# automatic packing of service projects to validate them
# (uses son-cli tools installed from *.deb package)

set -e

# ensure that the latest version of son-cli is installed
#sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-keys D0DF34A30A4FE3F8
#echo "deb http://registry.sonata-nfv.eu:8080 ubuntu-trusty main" | sudo tee -a /etc/apt/sources.list
sudo apt-get update
sudo apt-get remove -y sonata-cli
sudo apt-get autoremove
sudo apt-get install -y sonata-cli

# it might also be possible to run son-cli inside a dedicated Docker container. But the schema files seem to be missing.
#docker pull registry.sonata-nfv.eu:5000/son-cli
#docker run --rm -v $(pwd):/packages -w /packages registry.sonata-nfv.eu:5000/son-cli /bin/bash -c "son-workspace --init; son-package --project sonata-empty-service-emu -n sonata-empty-service"

# create a test workspace
son-workspace --init --workspace test_ws

# package all example service projects
son-package --workspace test_ws --project sonata-empty-service-emu -n sonata-empty-service
son-package --workspace test_ws --project sonata-snort-service-emu -n sonata-snort-service

# remove test workspace
rm -rf test_ws

# Note: The packaged services are not yet uploaded anywhere. We use packaging only to validate the service projects and their descriptors.