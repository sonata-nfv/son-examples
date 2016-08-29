#!/bin/sh

read -p "sudo password? " USERPASS
echo $USERPASS | sudo -S apt-get update
echo $USERPASS | sudo -S apt-get install -y git build-essential gcc libnuma-dev flex byacc libjson0-dev libcurl4-gnutls-dev jq dh-autoreconf libpcap-dev libpulse-dev libtool pkg-config



#==================================================================================================
#	1. Git
#==================================================================================================
echo $USERPASS | sudo -S apt-get update
echo $USERPASS | sudo -S apt-get -y install git



#==================================================================================================
#	2. Go (lang)
#==================================================================================================
wget "https://storage.googleapis.com/golang/go1.5.2.linux-amd64.tar.gz"		# download
echo $USERPASS | sudo -S tar -C /usr/local -xzf go1.5.2.linux-amd64.tar.gz	# extract files
echo 'export PATH=$PATH:/usr/local/go/bin' >> ~/.profile					# add /usr/local/go/bin to PATH
mkdir -pv ~/gowork/src/github.com											# create workspace folders
echo 'export GOPATH=$HOME/gowork' >> ~/.profile								# update GOPATH
echo 'export PATH=$PATH:$GOPATH/bin' >> ~/.profile							# update PATH
. ~/.profile 																# apply changes



#==================================================================================================
#	3. Beego (framework)
#==================================================================================================
go get github.com/astaxie/beego
go get github.com/beego/bee



#==================================================================================================
#	4. Clone PF_ring (web api & library bundled together)
#==================================================================================================
cd ~/gowork/src
git clone https://mnlab-ncsrd:ii70mseq@bitbucket.org/mnlab-ncsrd/pfring_web_api



#==================================================================================================
#	5. build PFring library
#==================================================================================================
cd ~/gowork/src/pfring_web_api/vtc
#Build nDPI library
cd nDPI
NDPI_DIR=$(pwd)
echo $NDPI_DIR
NDPI_INCLUDE=$(pwd)/src/include
echo $NDPI_INCLUDE
echo $USERPASS | sudo -S ./autogen.sh
echo $USERPASS | sudo -S ./configure
echo $USERPASS | sudo -S make
echo $USERPASS | sudo -S make install

#Build PF_RING library
cd ..
cd PF_RING
echo $USERPASS | sudo -S make
#Build PF_RING examples, including the modified pfbridge, with nDPI integrated.
cd userland/examples/
echo $USERPASS | sudo -S sed -i 's#EXTRA_LIBS =#EXTRA_LIBS='"${NDPI_DIR}"'/src/lib/.libs/libndpi.a -ljson-c -lcurl#' ./Makefile
echo $USERPASS | sudo -S sed -i 's# -Ithird-party# -Ithird-party/ -I'"$NDPI_INCLUDE"' -I'"$NDPI_DIR"'#' ./Makefile
echo $NDPI_DIR
echo $USERPASS | sudo -S make



#==================================================================================================
#	6. configure app (influxDB)
#==================================================================================================
read -p "Type the ip:port for influxDB (define IP address and avoid using 'localhost:port'): " INFLUXDB
echo "{ \"url\": \"http://$INFLUXDB\" }" > ~/gowork/src/pfring_web_api/controllers/influxDB.json




# configure cron to start the pfring-web-api after reboot
# first, create a folder where the script will be placed
echo $USERPASS | sudo -S mkdir -pv /usr/scripts
# write start.sh
printf '#!/bin/sh\n. ~/.profile\ncd ~/gowork/src/pfring_web_api\nexport USERPASS='$USERPASS'\nbee run watchall > allout.txt 2>&1\n' > ~/gowork/src/pfring_web_api/start.sh
echo $USERPASS | sudo -S cp ~/gowork/src/pfring_web_api/start.sh /usr/scripts/start_pfring.sh
echo $USERPASS | sudo -S chmod +x /usr/scripts/start_pfring.sh
# second, add the script to contrab
(crontab -u $USER -l; echo "@reboot /usr/scripts/start_pfring.sh") | crontab -u $USER -






# run
cd ~/gowork/src/pfring_web_api
bee run watchall
