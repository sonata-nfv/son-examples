
FROM ubuntu:14.04
MAINTAINER Stavros Kolometsos <stkolome@iit.demokritos.gr>

ENV IFMNG mgmt
ENV IFIN input
ENV IFOUT output

RUN apt-get update && apt-get install -y git net-tools inetutils-ping iproute screen ca-certificates git-core ssh automake build-essential libnuma-dev libpcap-dev pkg-config libtool libjson0-dev libcurl4-gnutls-dev curl git 
RUN apt-get clean -y
 
# Fixes empty home
ENV HOME /root

#Copy the files 
ADD Scripts /vTC/scripts
ADD vtc_dashboard /root/gowork/src/vtc_dashboard
ADD pfring_web_api /root/gowork/src/pfring_web_api
ADD start.sh /vTC/scripts
RUN chmod +x /vTC/scripts/start.sh

WORKDIR /vTC/scripts
RUN chmod +x pfring_install1.sh
RUN chmod +x pfring_install2.sh
RUN chmod +x dashboard_install1.sh
RUN chmod +x dashboard_install2.sh
RUN ./pfring_install1.sh
RUN ./pfring_install2.sh
RUN ./dashboard_install1.sh
RUN ./dashboard_install2.sh
#RUN ./start.sh
ENV SON_EMU_CMD ./start.sh

CMD /bin/bash
