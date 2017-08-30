#!/bin/bash
./ipconfig.sh > /mnt/share/ipconfig.log
#date > /mnt/share/start.txt

echo "ServerName 40.0.0.2" >> /etc/apache2/apache2.conf

service apache2 restart
