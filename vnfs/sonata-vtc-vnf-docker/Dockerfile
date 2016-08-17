FROM ubuntu:14.04
MAINTAINER Akis Kourtis <akis.kourtis@iit.demokritos.gr>
 
# Install dev tools
RUN apt-get update && apt-get install -y git net-tools inetutils-ping iproute ca-certificates git-core ssh automake build-essential libnuma-dev libpcap-dev pkg-config libtool libjson0-dev libcurl4-gnutls-dev
RUN apt-get clean -y
 
# Fixes empty home
ENV HOME /root

# Copy the files
ADD PF_RING /vTC/PF_RING
ADD nDPI /vTC/nDPI

#WORKDIR /vTC/PF_RING/kernel
#RUN make

# build
WORKDIR /vTC/nDPI
RUN ./autogen.sh
RUN ./configure
RUN make
RUN make install
 
WORKDIR /PF_RING/userland/examples
#RUN make
