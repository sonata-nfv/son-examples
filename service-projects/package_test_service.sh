#!/bin/bash

# create a test workspace
son-workspace --init --workspace test_ws

# Note: The packaged services are not yet uploaded anywhere. We use packaging only to validate the service projects and their descriptors.
son-package --workspace test_ws --project sonata-sdk-test-service-emu -n sonata-sdk-test-service

# leave venv
deactivate

# remove test workspace
rm -rf test_ws

# remove son-cli
rm -rf son-cli


