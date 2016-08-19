FROM ubuntu:xenial
MAINTAINER manuel.peuster@uni-paderborn.de

RUN	apt-get update && apt-get install -y \
	net-tools \
	iproute \
	inetutils-ping \
	iperf3 \
	nmap \
	ssh

# set entry point for emulator gatekeeper
ENV SON_EMU_CMD echo "iperf3 VNF ready"

CMD /bin/bash