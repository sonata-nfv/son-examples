#!/bin/sh

read -p "sudo password:" USERPASS



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
#	4. influxDB
#==================================================================================================
wget http://influxdb.s3.amazonaws.com/influxdb_0.9.5.1_amd64.deb				# download
echo $USERPASS | sudo -S dpkg -i influxdb_0.9.5.1_amd64.deb						# install
echo $USERPASS | sudo -S service influxdb start									# start service
sleep 3s
echo $USERPASS | sudo -S curl -G http://localhost:8086/query --data-urlencode "q=CREATE DATABASE flows"	# create db 'flows'



#==================================================================================================
#	5. Grafana
#==================================================================================================
wget https://grafanarel.s3.amazonaws.com/builds/grafana_2.5.0_amd64.deb		# download
echo $USERPASS | sudo -S apt-get install -y adduser libfontconfi
echo $USERPASS | sudo -S dpkg -i grafana_2.5.0_amd64.deb
echo $USERPASS | sudo -S update-rc.d grafana-server defaults 95 10			# To configure the Grafana server to start at boot time
echo $USERPASS | sudo -S service grafana-server start						# start service



#==================================================================================================
#	6. app source code
#==================================================================================================
cd ~/gowork/src/
git clone https://mnlab-ncsrd:ii70mseq@bitbucket.org/mnlab-ncsrd/vtc_dashboard.git



#==================================================================================================
#	7. configure app (Grafana, VTC host)
#==================================================================================================
cd ~/gowork/src/vtc_dashboard/static/json/
# add data source and initialize dashboard using the Grafana API:
curl -XPOST --data-binary @grafana_init_datasources.json -H 'Content-Type:application/json' -H 'Accept: application/json' http://admin:admin@localhost:3000/api/datasources
curl -XPOST --data-binary @grafana_init_dashboard.json -H 'Content-Type:application/json' -H 'Accept: application/json' http://admin:admin@localhost:3000/api/dashboards/db
curl -X POST -d '{"role":"Admin","name":"apikey"}' -H 'Content-Type:application/json' -H 'Accept: application/json' http://admin:admin@localhost:3000/api/auth/keys > ~/gowork/src/vtc_dashboard/controllers/grafana_key.json

read -p "Type the ip:port for Grafana (define IP address and avoid using 'localhost:port'): " GRAFANA
echo "{ \"url\": \"http://$GRAFANA\" }" > ~/gowork/src/vtc_dashboard/static/json/grafana.json

read -p "Type the ip:port for PFring API: " PFRING
echo "{ \"url\": \"http://$PFRING\" }" > ~/gowork/src/vtc_dashboard/static/json/vtcHost.json


# configure cron to start the vtc-dashboard after reboot
# first, create a folder where the script will be placed
echo $USERPASS | sudo -S mkdir -pv /usr/scripts
echo $USERPASS | sudo -S cp ~/gowork/src/vtc_dashboard/start.sh /usr/scripts/start_vtc.sh
echo $USERPASS | sudo -S chmod +x /usr/scripts/start_vtc.sh
# second, add the script to contrab
(crontab -u $USER -l; echo "@reboot /usr/scripts/start_vtc.sh") | crontab -u $USER -


# run
cd ~/gowork/src/vtc_dashboard
bee run watchall
