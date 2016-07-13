FROM ubuntu:xenial
MAINTAINER manuel.peuster@uni-paderborn.de

RUN apt-get update && apt-get install -y \
	iptables \
	arptables \
	ebtables \
	bridge-utils \
	ethtool \
	snort

ADD snort.conf /etc/snort/snort.conf
ADD start.sh start.sh
RUN chmod +x start.sh

CMD /bin/bash
