     #==================================================================================================
      #	  7. configure app (Grafana, VTC host)
      #==================================================================================================

cd /root/gowork/src/vtc_dashboard/static/json/
IPP="localhost" #$(ip route get 8.8.8.8 | awk '{print $NF; exit}')
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
