#!/bin/sh
#set -e # we want to stop on error
apt-get -y install wget
apt-get -y install curl
apt-get -y install make 
apt-get install -y git build-essential gcc libnuma-dev flex byacc libjson0-dev libcurl4-gnutls-dev jq dh-autoreconf libpcap-dev libpulse-dev libtool pkg-config

printf "auto eth1\niface eth1 inet dhcp\n" >> /etc/network/interfaces
printf "auto eth2\niface eth2 inet dhcp\n" >> /etc/network/interfaces
ifup eth1
ifup eth2

      #==================================================================================================
      #	  2. Go (lang)
      #==================================================================================================
wget "https://storage.googleapis.com/golang/go1.5.2.linux-amd64.tar.gz"			# download
tar -C /usr/local -xzf go1.5.2.linux-amd64.tar.gz								# extract files
echo 'export PATH=$PATH:/usr/local/go/bin' >> /.profile					# add /usr/local/go/bin to PATH
mkdir -pv /root/gowork/src/github.com											          # create workspace folders
echo 'export GOPATH=/root/gowork' >> /.profile								        # update GOPATH
echo 'export PATH=$PATH:$GOPATH/bin' >> /.profile						   	# update PATH
. /.profile 															                    	# apply changes
    

      #==================================================================================================
      #	  3. Beego (framework)
      #==================================================================================================
go get github.com/astaxie/beego
go get github.com/beego/bee
    
    
      #==================================================================================================
      #   4. Clone PF_ring (web api & library bundled together)
      #==================================================================================================
cd /root/gowork/src
git clone https://mnlab-ncsrd:mnlabs0nata@bitbucket.org/mnlab-ncsrd/pfring_web_api
    
    
      #==================================================================================================
      #   5. build PFring library
      #==================================================================================================
cd /root/gowork/src/pfring_web_api/vtc
      #Build nDPI library
cd nDPI
NDPI_DIR=$(pwd)
echo $NDPI_DIR
NDPI_INCLUDE=$(pwd)/src/include
echo $NDPI_INCLUDE
./autogen.sh
./configure
make
make install
#Build PF_RING library
cd ..
cd PF_RING
make
      Build PF_RING examples, including the modified pfbridge, with nDPI integrated.
cd userland/examples/
sed -i 's#EXTRA_LIBS =#EXTRA_LIBS='"${NDPI_DIR}"'/src/lib/.libs/libndpi.a -ljson-c -lcurl#' ./Makefile
sed -i 's# -Ithird-party# -Ithird-party/ -I'"$NDPI_INCLUDE"' -I'"$NDPI_DIR"'#' ./Makefile
echo $NDPI_DIR
make

      #==================================================================================================
      #   6. configure app (influxDB)
      #==================================================================================================
      #read -p "Type the ip:port for influxDB (define IP address and avoid using 'localhost:port'): " INFLUXDB
IPP=$(ip route get 8.8.8.8 | awk '{print $NF; exit}')
echo "{ \"url\": \"http://"$IPP":8086\" }" > /root/gowork/src/pfring_web_api/controllers/influxDB.json
      # configure cron to start the pfring-web-api after reboot
      # first, create a folder where the script will be placed
mkdir -pv /usr/scripts
      # write start.sh
printf '#!/bin/sh\n. /.profile\ncd /root/gowork/src/pfring_web_api\nbee run watchall > allout.txt 2>&1\n' > /root/gowork/src/pfring_web_api/start.sh
cp /root/gowork/src/pfring_web_api/start.sh /usr/scripts/start_pfring.sh
chmod +x /usr/scripts/start_pfring.sh
      echo "appname = checkPFringModule
httpport = 8090
runmode = dev" > /root/gowork/src/pfring_web_api/conf/app.conf
      # second, add the script to contrab
(crontab -l; echo "@reboot /usr/scripts/start_pfring.sh") | crontab -
# run
cd /root/gowork/src/pfring_web_api
bee run watchall &
