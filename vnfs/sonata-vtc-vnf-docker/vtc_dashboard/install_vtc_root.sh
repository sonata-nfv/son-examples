#!/bin/sh
#==================================================================================================
#	1. Git
#==================================================================================================
apt-get update
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
#	4. influxDB
#==================================================================================================
wget http://influxdb.s3.amazonaws.com/influxdb_0.9.5.1_amd64.deb				# download
dpkg -i influxdb_0.9.5.1_amd64.deb						# install
service influxdb start									# start service
sleep 3s
curl -G http://localhost:8086/query --data-urlencode "q=CREATE DATABASE flows"	# create db 'flows'



#==================================================================================================
#	5. Grafana
#==================================================================================================
wget https://grafanarel.s3.amazonaws.com/builds/grafana_2.5.0_amd64.deb		# download
apt-get install -y adduser libfontconfig
dpkg -i grafana_2.5.0_amd64.deb
update-rc.d grafana-server defaults 95 10			# To configure the Grafana server to start at boot time
service grafana-server start						# start service



#==================================================================================================
#	6. app source code
#==================================================================================================
cd $HOME/gowork/src/
git clone https://mnlab-ncsrd:ii70mseq@bitbucket.org/mnlab-ncsrd/vtc_dashboard.git



#==================================================================================================
#	7. configure app (Grafana, VTC host)
#==================================================================================================
cd $HOME/gowork/src/vtc_dashboard/static/json/
# add data source and initialize dashboard using the Grafana API:
curl -XPOST --data-binary @grafana_init_datasources.json -H 'Content-Type:application/json' -H 'Accept: application/json' http://admin:admin@localhost:3000/api/datasources
curl -XPOST --data-binary @grafana_init_dashboard.json -H 'Content-Type:application/json' -H 'Accept: application/json' http://admin:admin@localhost:3000/api/dashboards/db
curl -X POST -d '{"role":"Admin","name":"apikey"}' -H 'Content-Type:application/json' -H 'Accept: application/json' http://admin:admin@localhost:3000/api/auth/keys > $HOME/gowork/src/vtc_dashboard/controllers/grafana_key.json

read -p "Type the ip:port for Grafana (define IP address and avoid using 'localhost:port'): " GRAFANA
echo "{ \"url\": \"http://$GRAFANA\" }" > $HOME/gowork/src/vtc_dashboard/static/json/grafana.json

read -p "Type the ip:port for PFring API: " PFRING
echo "{ \"url\": \"http://$PFRING\" }" > $HOME/gowork/src/vtc_dashboard/static/json/vtcHost.json


# configure cron to start the vtc-dashboard after reboot
# first, create a folder where the script will be placed
mkdir -pv /usr/scripts
cp $HOME/gowork/src/vtc_dashboard/start.sh /usr/scripts/start_vtc.sh
chmod +x /usr/scripts/start_vtc.sh
# second, add the script to contrab
(crontab -l; echo "@reboot /usr/scripts/start_vtc.sh") | crontab -


# run
cd $HOME/gowork/src/vtc_dashboard
bee run watchall
