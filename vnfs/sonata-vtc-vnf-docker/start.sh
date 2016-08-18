#!/bin/bash
#
# entry point of vTC container

sleep 1

# run pfbridge with interfaces specified in Dockerfile
./pfbridge -a $IFIN -b $IFOUT