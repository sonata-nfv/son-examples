FROM mpeuster/sonata-pcap-container:latest
MAINTAINER steven.vanrossem@intec.ugent.be

RUN	apt-get update && apt-get install -y \
	net-tools \
	iproute \
	inetutils-ping \
	iperf3 \
	tcpreplay \
	nmap \
	curl \
	tcpdump \
	bmon \
	netcat \
	ssh \
        ethtool

RUN apt-get install -y software-properties-common openssh-server
RUN echo 'root:root' | chpasswd
RUN sed -i 's/PermitRootLogin without-password/PermitRootLogin yes/' /etc/ssh/sshd_config
ADD authorized_keys /root/.ssh/authorized_keys
# set permissions correctly, otherwise ssh will not be able to login via ssh keys 
RUN chmod 700 /root/.ssh
RUN chmod 600 /root/.ssh/authorized_keys

# expose port 22 for ssh 
EXPOSE 22

# install prometheus_client so we can export metrics to Prometheus Pushgateway
RUN apt-get install -y python-pip
RUN pip install prometheus_client

RUN apt-get install -y iperf

RUN apt-get install -y tcpdump
RUN mv /usr/sbin/tcpdump /usr/bin/tcpdump

ADD iperf_server.py iperf_server.py
RUN chmod +x iperf_server.py
ADD start_iperf.sh start_iperf.sh
RUN chmod +x iperf_server.py

ADD start_sap.sh start.sh
RUN chmod +x start.sh

# prepare pcap replay for SONATA Y1 demo
WORKDIR /pcap
RUN tcpprep --auto=client --pcap=getall.pcap --cachefile=input.cache
RUN tcprewrite --endpoints=203.0.0.2:203.0.0.1 --skipbroadcast --cachefile=input.cache -i getall.pcap -o output.pcap

WORKDIR /

ADD start_pcap_replay.sh start_pcap_replay.sh
RUN chmod +x start_pcap_replay.sh

# set entry point for emulator gatekeeper
ENV SON_EMU_CMD "./start.sh"

CMD /bin/bash
#ENTRYPOINT [ "python", "./iperf_server.py"]
