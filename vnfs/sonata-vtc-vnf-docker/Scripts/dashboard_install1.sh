#!/bin/bash
apt-get -y install openssh-client
apt-get -y install openssh-server
apt-get -y install curl
      #==================================================================================================
      #   1. Git
      #==================================================================================================

apt-get -y install git

      #==================================================================================================
      #   2. Go (lang)
      #==================================================================================================

wget "https://storage.googleapis.com/golang/go1.5.2.linux-amd64.tar.gz"		# download
tar -C /usr/local -xzf go1.5.2.linux-amd64.tar.gz								# extract files
echo 'export PATH=$PATH:/usr/local/go/bin' >> /.profile					# add /usr/local/go/bin to PATH
mkdir -pv /root/gowork/src/github.com									          		# create workspace folders
echo 'export GOPATH=/root/gowork' >> /.profile			          				# update GOPATH
echo 'export PATH=$PATH:$GOPATH/bin' >> /.profile			    			# update PATH
. /.profile 														                    		# apply changes

      #==================================================================================================
      #   3. Beego (framework)
      #==================================================================================================

go get github.com/astaxie/beego
go get github.com/beego/bee

      #==================================================================================================
      #   4. influxDB
      #==================================================================================================

wget http://influxdb.s3.amazonaws.com/influxdb_0.9.5.1_amd64.deb					# download
dpkg -i influxdb_0.9.5.1_amd64.deb												# install
service influxdb start															# start service
sleep 10s
curl -G http://localhost:8086/query --data-urlencode "q=CREATE DATABASE flows"	# create db 'flows'

      #==================================================================================================
      #   5. Grafana
      #==================================================================================================

wget https://grafanarel.s3.amazonaws.com/builds/grafana_2.5.0_amd64.deb		# download
apt-get install -y adduser libfontconfig
dpkg -i grafana_2.5.0_amd64.deb
service grafana-server start						# start service
sudo netstat -tnlp > netstat.txt
update-rc.d grafana-server defaults 95 10     # To configure the Grafana server to start at boot time
cd /root/gowork/src/vtc_dashboard

printf "0\n-------------------\n" > netstat.txt
netstat -tnlp >> netstat.txt
sleep 10s

      #==================================================================================================
      #
      #===============================================================================================
