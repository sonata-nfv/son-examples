FROM ubuntu:xenial
MAINTAINER edmaas@mail.uni-paderborn.de

RUN	apt-get update && apt-get install -y \
	net-tools \
	iproute \
	inetutils-ping \
    libapr1 \
    libaprutil1 \
    libc6 \
    libssl1.0.0 \
    apache2 \
    curl

ADD start.sh start.sh
RUN chmod +x start.sh
ADD stop.sh stop.sh
RUN chmod +x stop.sh
ADD ipconfig.sh ipconfig.sh
RUN chmod +x ipconfig.sh

# set entry point for emulator gatekeeper
ENV SON_EMU_CMD "./start.sh"
ENV SON_EMU_CMD_STOP "./stop.sh"

CMD /bin/bash
