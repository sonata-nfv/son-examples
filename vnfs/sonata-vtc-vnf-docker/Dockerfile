FROM ubuntu:14.04
MAINTAINER Akis Kourtis <akis.kourtis@iit.demokritos.gr> and Stavros Kolometsos <stkolome@iit.demokritos.gr>

ENV IFMNG mgmt
ENV IFIN input
ENV IFOUT output

RUN apt-get update && apt-get install -y git net-tools inetutils-ping iproute screen ca-certificates git-core ssh automake build-essential libnuma-dev libpcap-dev pkg-config libtool libjson0-dev libcurl4-gnutls-dev curl git 
RUN apt-get clean -y
 
# Fixes empty home
ENV HOME /root

#Copy the files 
ADD Scripts /vTC/scripts

#Install pfring and dashboard 
WORKDIR /vTC/scripts
RUN chmod +x pfring_install.sh
RUN chmod +x dashboard_install.sh
RUN ./pfring_install.sh
RUN ./dashboard_install.sh

# set entry point for emulator gatekeeper
ENV SON_EMU_CMD ./start.sh

# normal entry point
CMD /bin/bash
