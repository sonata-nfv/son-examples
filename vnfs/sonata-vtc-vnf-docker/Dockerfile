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
ADD Scripts /root/gowork/scripts
ADD vtc_dashboard /root/gowork/src/vtc_dashboard
ADD pfring_web_api /root/gowork/src/pfring_web_api
ADD start.sh /root/gowork/scripts
RUN chmod +x /root/gowork/scripts/start.sh

WORKDIR /root/gowork/scripts
RUN chmod +x pfring_install1.sh
RUN chmod +x pfring_install2.sh
RUN chmod +x dashboard_install1.sh
RUN chmod +x dashboard_install2.sh
RUN bash -e pfring_install1.sh
RUN bash  pfring_install2.sh
RUN bash -e dashboard_install1.sh
RUN bash  dashboard_install2.sh
#RUN ./start.sh
ENV SON_EMU_CMD ./start.sh
