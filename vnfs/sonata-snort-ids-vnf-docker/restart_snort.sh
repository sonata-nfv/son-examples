#!/bin/bash

pkill snort
sleep 1

# run snort as background process
snort -i br0 -D -q -k none -K ascii -l /snort-logs -A fast -c /etc/snort/snort.conf

echo "Snort VNF started ..."