FROM      osrg/ryu
MAINTAINER steven.vanrossem@intec.ugent.be

RUN	apt-get update && apt-get install -y \
	net-tools \
	iproute \
	inetutils-ping

# install OpenvSwitch 
RUN apt-get install -y openvswitch-switch openvswitch-common 


ADD start_fw.sh start.sh
RUN chmod +x start.sh


# set entry point for emulator gatekeeper
ENV SON_EMU_CMD "./start.sh"

CMD /bin/bash
