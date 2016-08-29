#!/bin/sh

apt-get update
apt-get install -y git build-essential gcc libnuma-dev flex byacc libjson0-dev libcurl4-gnutls-dev jq dh-autoreconf libpcap-dev libpulse-dev libtool pkg-config



#==================================================================================================
#	1. Git
#==================================================================================================
apt-get -y install git



#==================================================================================================
#	2. Go (lang)
#==================================================================================================
wget "https://storage.googleapis.com/golang/go1.5.2.linux-amd64.tar.gz"		# download
tar -C /usr/local -xzf go1.5.2.linux-amd64.tar.gz	# extract files
echo 'export PATH=$PATH:/usr/local/go/bin' >> $HOME/.profile					# add /usr/local/go/bin to PATH
mkdir -pv $HOME/gowork/src/github.com											# create workspace folders
echo 'export GOPATH=$HOME/gowork' >> $HOME/.profile								# update GOPATH
echo 'export PATH=$PATH:$GOPATH/bin' >> $HOME/.profile							# update PATH
. $HOME/.profile 																# apply changes



#==================================================================================================
#	3. Beego (framework)
#==================================================================================================
go get github.com/astaxie/beego
go get github.com/beego/bee



#==================================================================================================
#	4. Clone PF_ring (web api & library bundled together)
#==================================================================================================
cd $HOME/gowork/src
git clone https://mnlab-ncsrd:ii70mseq@bitbucket.org/mnlab-ncsrd/pfring_web_api



#==================================================================================================
#	5. build PFring library
#==================================================================================================
cd $HOME/gowork/src/pfring_web_api/vtc
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
#Build PF_RING examples, including the modified pfbridge, with nDPI integrated.
cd userland/examples/
sed -i 's#EXTRA_LIBS =#EXTRA_LIBS='"${NDPI_DIR}"'/src/lib/.libs/libndpi.a -ljson-c -lcurl#' ./Makefile
sed -i 's# -Ithird-party# -Ithird-party/ -I'"$NDPI_INCLUDE"' -I'"$NDPI_DIR"'#' ./Makefile
echo $NDPI_DIR
make



#==================================================================================================
#	6. configure app (influxDB)
#==================================================================================================
read -p "Type the ip:port for influxDB (define IP address and avoid using 'localhost:port'): " INFLUXDB
echo "{ \"url\": \"http://$INFLUXDB\" }" > $HOME/gowork/src/pfring_web_api/controllers/influxDB.json




# configure cron to start the pfring-web-api after reboot
# first, create a folder where the script will be placed
mkdir -pv /usr/scripts
# write start.sh
printf '#!/bin/sh\n. $HOME/.profile\ncd $HOME/gowork/src/pfring_web_api\nexport USERPASS='$USERPASS'\nbee run watchall > allout.txt 2>&1\n' > $HOME/gowork/src/pfring_web_api/start.sh
cp $HOME/gowork/src/pfring_web_api/start.sh /usr/scripts/start_pfring.sh
chmod +x /usr/scripts/start_pfring.sh
# second, add the script to contrab
(crontab -u $USER -l; echo "@reboot /usr/scripts/start_pfring.sh") | crontab -u $USER -






# run
cd $HOME/gowork/src/pfring_web_api
bee run watchall