#!/bin/bash

# CI entry point
# automatic packing of service projects to validate them
# (uses son-cli tools installed directly from git)

set -e

git clone https://github.com/sonata-nfv/son-cli.git
cd son-cli
virtualenv -p /usr/bin/python3.4 venv
source venv/bin/activate
python bootstrap.py
bin/buildout
python setup.py develop
cd ..

# create a test workspace
son-workspace --init --workspace test_ws

# package all example service projects
which son-package
son-package --workspace test_ws --project sonata-empty-service-emu -n sonata-empty-service
son-package --workspace test_ws --project sonata-snort-service-emu -n sonata-snort-service

# leave venv
deactivate

# remove test workspace
rm -rf test_ws

# remove son-cli
rm -rf son-cli

# Note: The packaged services are not yet uploaded anywhere. We use packaging only to validate the service projects and their descriptors.