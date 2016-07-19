FROM ubuntu:xenial
MAINTAINER manuel.peuster@uni-paderborn.de

RUN	apt-get update && apt-get install -y \
	net-tools \
	iproute \
	inetutils-ping

# set entry point for emulator gatekeeper
ENV SON_EMU_CMD "echo Hello World!"

CMD /bin/bash