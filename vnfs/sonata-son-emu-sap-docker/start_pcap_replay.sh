#!/bin/bash

cd /pcap
tcpreplay -i servicein --loop=0 output.pcap