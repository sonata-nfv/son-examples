#!/bin/bash
#
# entry point of vTC container

. /.profile
IPP=$(ip route get 8.8.8.8 | awk '{print $NF; exit}')

#Starts the services 

service influxdb start
#read -p "Type the ip:port for influxDB (define IP address and avoid using 'localhost:port'): " INFLUXDB
echo "{ \"url\": \"http://"$IPP":8086\" }" > /root/gowork/src/pfring_web_api/controllers/influxDB.json
# run
cd /root/gowork/src/pfring_web_api
bee run watchall &
cd /root/gowork/src/vtc_dashboard/static/json/
# add data source and initialize dashboard using the Grafana API:
sed -i 's/localhost/'$IPP'/g' grafana_init_datasources.json
service grafana-server start
curl -X POST --connect-timeout 60 --data-binary @grafana_init_datasources.json -H 'Content-Type:application/json' -H 'Accept: application/json' http://admin:admin@$IPP:3000/api/datasources
sleep 5s
curl -X POST --connect-timeout 60 --data-binary @grafana_init_dashboard.json -H 'Content-Type:application/json' -H 'Accept: application/json' http://admin:admin@$IPP:3000/api/dashboards/db
sleep 5s
curl -X POST --connect-timeout 60 -d '{"role":"Admin","name":"apikey"}' -H 'Content-Type:application/json' -H 'Accept: application/json' http://admin:admin@$IPP:3000/api/auth/keys > /root/gowork/src/vtc_dashboard/controllers/grafana_key.json
echo "{ \"url\": \"http://"$IPP":3000\" }" > /root/gowork/src/vtc_dashboard/static/json/grafana.json
#read -p "Type the ip:port for PFring API: " PFRING
echo "{ \"url\": \"http://"$IPP":8090\" }" > /root/gowork/src/vtc_dashboard/static/json/vtcHost.json
# run
cd /root/gowork/src/vtc_dashboard
bee run watchall &


# remove IPs from input/output interface to prepare them for bridging
ip addr flush dev $IFIN
ip addr flush dev $IFOUT

sleep 1

# run pfbridge with interfaces specified in Dockerfile (run non-blocking!)
cd /root/gowork/src/pfring_web_api/vtc/PF_RING/userland/examples
screen -d -m -s "pfbridge" ./pfbridge -a $IFIN -b $IFOUT -d "http://"$IPP":8086"


echo "VTC_VNF: pfbrdige was started in a screen session"

