#!/bin/bash
./ipconfig.sh > /mnt/share/ipconfig.log

date > /mnt/share/start.txt

# modify config using ENV vars
sed -i 's/TCP_LISTEN_PORT/'"$TCP_LISTEN_PORT"'/g' /etc/nginx/nginx.conf
sed -i 's/TCP_SERVERS/'"$TCP_SERVERS"'/g' /etc/nginx/nginx.conf
sed -i 's/UDP_LISTEN_PORT/'"$UDP_LISTEN_PORT"'/g' /etc/nginx/nginx.conf
sed -i 's/UDP_SERVERS/'"$UDP_SERVERS"'/g' /etc/nginx/nginx.conf


nginx &

echo "Nginx VNF started ..."



