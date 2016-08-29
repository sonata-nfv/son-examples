#!/bin/bash
#set -e # we want to stop on error
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
sleep 3s
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
sleep 5s

      #==================================================================================================
      #	  6. app source code
      #==================================================================================================

cd /root/gowork/src/
git clone https://mnlab-ncsrd:mnlabs0nata@bitbucket.org/mnlab-ncsrd/vtc_dashboard.git

      #==================================================================================================
      #	  7. configure app (Grafana, VTC host)
      #==================================================================================================

cd /root/gowork/src/vtc_dashboard/static/json/
IPP=$(ip route get 8.8.8.8 | awk '{print $NF; exit}')
# add data source and initialize dashboard using the Grafana API:
sed -i 's/localhost/'$IPP'/g' grafana_init_datasources.json
service grafana-server status
sudo netstat -tnlp > netstat.txt
curl -X POST --connect-timeout 60 --data-binary @grafana_init_datasources.json -H 'Content-Type:application/json' -H 'Accept: application/json' http://admin:admin@$IPP:3000/api/datasources
sleep 5s
curl -X POST --connect-timeout 60 --data-binary @grafana_init_dashboard.json -H 'Content-Type:application/json' -H 'Accept: application/json' http://admin:admin@$IPP:3000/api/dashboards/db
sleep 5s
curl -X POST --connect-timeout 60 -d '{"role":"Admin","name":"apikey"}' -H 'Content-Type:application/json' -H 'Accept: application/json' http://admin:admin@$IPP:3000/api/auth/keys > /root/gowork/src/vtc_dashboard/controllers/grafana_key.json
echo "{ \"url\": \"http://"$IPP":3000\" }" > /root/gowork/src/vtc_dashboard/static/json/grafana.json
#read -p "Type the ip:port for PFring API: " PFRING
echo "{ \"url\": \"http://"$IPP":8090\" }" > /root/gowork/src/vtc_dashboard/static/json/vtcHost.json
# configure cron to start the vtc-dashboard after reboot
# first, create a folder where the script will be placed
mkdir -pv /usr/scripts
cp /root/gowork/src/vtc_dashboard/start.sh /usr/scripts/start_vtc.sh
chmod +x /usr/scripts/start_vtc.sh
# second, add the script to contrab
(crontab -l; echo "@reboot /usr/scripts/start_vtc.sh") | crontab -
# run
cd /root/gowork/src/vtc_dashboard
bee run watchall &
