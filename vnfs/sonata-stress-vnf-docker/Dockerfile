FROM ubuntu:xenial
MAINTAINER manuel.peuster@uni-paderborn.de, edmaas@mail.upb.de

RUN	apt-get update && apt-get install -y \
	net-tools \
	iproute \
    stress

ADD start.sh start.sh
RUN chmod +x start.sh
ADD stop.sh stop.sh
RUN chmod +x stop.sh

ENV SON_EMU_CMD "./start.sh"

CMD /bin/bash
