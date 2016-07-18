#!/bin/bash

#
# This is a helper script to automatically pack (ZIP) the example services as they are.
# However, this is only considered to be a helper for some scripts. You should 
# use son-package instead.
#
set -e

cd sonata-empty-demo; zip -r ../sonata-empty-demo.son *; cd ..
cd sonata-snort-service; zip -r ../sonata-snort-service.son *; cd ..