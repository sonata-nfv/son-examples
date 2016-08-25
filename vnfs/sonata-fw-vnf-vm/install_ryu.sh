#!/bin/bash
	   
# Install pip
#apt-get update
apt-get install -y python-pip python-dev build-essential 
pip install --upgrade pip 
pip install --upgrade six

# Download and install Ryu:
pip install ryu


