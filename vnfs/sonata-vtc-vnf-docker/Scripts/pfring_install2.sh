 #==================================================================================================
      #   5. build PFring library
      #==================================================================================================
      cd /root/gowork/src/pfring_web_api/vtc
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
      #   6. configure app (influxDB)
      #==================================================================================================
      #read -p "Type the ip:port for influxDB (define IP address and avoid using 'localhost:port'): " INFLUXDB
      IPP=$(ip route get 8.8.8.8 | awk '{print $NF; exit}')
      echo "{ \"url\": \"http://"$IPP":8086\" }" > /root/gowork/src/pfring_web_api/controllers/influxDB.json
      # configure cron to start the pfring-web-api after reboot
      # first, create a folder where the script will be placed
      mkdir -pv /usr/scripts
      # write start.sh
      printf '#!/bin/sh\n. /.profile\ncd /root/gowork/src/pfring_web_api\nbee run watchall > allout.txt 2>&1\n' > /root/gowork/src/pfring_web_api/start.sh
      cp /root/gowork/src/pfring_web_api/start.sh /usr/scripts/start_pfring.sh
      chmod +x /usr/scripts/start_pfring.sh
      echo "appname = checkPFringModule
httpport = 8090 
runmode = dev" > /root/gowork/src/pfring_web_api/conf/app.conf
      # second, add the script to contrab
