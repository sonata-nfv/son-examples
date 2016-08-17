FROM ubuntu:14.04
MAINTAINER Akis Kourtis <akis.kourtis@iit.demokritos.gr>
 
# Install dev tools
RUN apt-get update && apt-get install -y git net-tools inetutils-ping iproute ca-certificates git-core ssh automake build-essential libnuma-dev libpcap-dev pkg-config libtool libjson0-dev libcurl4-gnutls-dev
RUN apt-get clean -y
 
# Fixes empty home
ENV HOME /root

 
# Install the app
RUN git clone https://akiskourtis:ptindpi@bitbucket.org/akiskourtis/dockerdpi.git

#WORKDIR dockerdpi/PF_RING/kernel
#RUN make

WORKDIR dockerdpi/nDPI
RUN ./autogen.sh
RUN ./configure
RUN make
RUN make install

#RUN insmod ./pf_ring.ko
 
WORKDIR ../PF_RING/userland/examples
#RUN make
