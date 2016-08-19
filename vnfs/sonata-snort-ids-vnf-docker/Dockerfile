FROM ubuntu:xenial
MAINTAINER manuel.peuster@uni-paderborn.de

# define interface names (should be the same as in VNFD)
ENV IFIN input
ENV IFOUT output

RUN export DEBIAN_FRONTEND=noninteractive && apt-get update && apt-get install -y -q \
	net-tools \
	iproute \
	inetutils-ping \
	iptables \
	arptables \
	ebtables \
	bridge-utils \
	ethtool \
	vim \
	snort

ADD snort.conf /etc/snort/snort.conf
ADD start.sh start.sh
RUN chmod +x start.sh
ADD restart_snort.sh restart_snort.sh
RUN chmod +x restart_snort.sh
RUN mkdir /snort-logs

# set entry point for emulator gatekeeper
ENV SON_EMU_CMD ./start.sh

# normal entry point
CMD /bin/bash
