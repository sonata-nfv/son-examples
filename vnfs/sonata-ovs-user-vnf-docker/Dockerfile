FROM ubuntu:14.04
MAINTAINER steven.vanrossem@intec.ugent.be

RUN	apt-get update && apt-get install -y \
	net-tools \
	iproute \
	inetutils-ping \
	wget

# install OpenvSwitch 
#RUN apt-get install -y openvswitch-switch openvswitch-common 


# Get Open vSwitch
RUN mkdir -p /etc/openvswitch
ENV OVS_VERSION 2.4.0
WORKDIR /
RUN wget https://s3-us-west-2.amazonaws.com/docker-ovs/openvswitch-$OVS_VERSION.tar.gz --no-check-certificate && \
	tar -xzvf openvswitch-$OVS_VERSION.tar.gz &&\
	mv openvswitch-$OVS_VERSION openvswitch &&\
	cp -r openvswitch/* / &&\
	rm -r openvswitch &&\
	rm openvswitch-$OVS_VERSION.tar.gz 
ADD configure-ovs.sh /usr/local/share/openvswitch/
RUN chmod +x /usr/local/share/openvswitch/configure-ovs.sh
# Create the database
RUN ovsdb-tool create /etc/openvswitch/conf.db /usr/local/share/openvswitch/vswitch.ovsschema


ADD start_ovs1.sh start_ovs1.sh
RUN chmod +x start_ovs1.sh
ADD configure_ovs1.sh configure.sh
RUN chmod +x configure.sh
ADD start.sh start.sh
RUN chmod +x start.sh

# set entry point for emulator gatekeeper
ENV SON_EMU_CMD "bash start.sh"


CMD /bin/bash
