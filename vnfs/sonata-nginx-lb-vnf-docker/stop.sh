#! /bin/bash
pkill nginx
sleep 2

python /log_intf_statistics.py /mnt/share/result.yml


date > /mnt/share/stop.txt
