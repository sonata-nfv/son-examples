FROM ubuntu:xenial
MAINTAINER manuel.peuster@uni-paderborn.de

RUN	apt-get update && apt-get install -y \
	software-properties-common \
	aptitude

RUN add-apt-repository "ppa:patrickdk/general-lucid" -y
RUN apt-get update && apt-get install -y \
	iperf3 \
	nmap

CMD /bin/bash