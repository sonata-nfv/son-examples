FROM      osrg/ryu
MAINTAINER steven.vanrossem@intec.ugent.be

RUN	apt-get update && apt-get install -y \
	net-tools \
	iproute \
	inetutils-ping


ADD start_ryu_snort.sh start.sh
RUN chmod +x start.sh

ADD simple_switch_snort_sonata.py simple_switch_snort_sonata.py

# set entry point for emulator gatekeeper
ENV SON_EMU_CMD "./start.sh"

CMD /bin/bash
