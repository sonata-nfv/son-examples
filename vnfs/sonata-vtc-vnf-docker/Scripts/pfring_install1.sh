#!/bin/sh
      apt-get -y install wget
      apt-get -y install curl
      apt-get -y install make 
      apt-get install -y git build-essential gcc libnuma-dev flex byacc libjson0-dev libcurl4-gnutls-dev jq dh-autoreconf libpcap-dev libpulse-dev libtool pkg-config

      printf "auto eth1\niface eth1 inet dhcp\n" >> /etc/network/interfaces
      printf "auto eth2\niface eth2 inet dhcp\n" >> /etc/network/interfaces
      ifup eth1
      ifup eth2

      #==================================================================================================
      #	  2. Go (lang)
      #==================================================================================================
      wget "https://storage.googleapis.com/golang/go1.5.2.linux-amd64.tar.gz"			# download
      tar -C /usr/local -xzf go1.5.2.linux-amd64.tar.gz								# extract files
      echo 'export PATH=$PATH:/usr/local/go/bin' >> /.profile					# add /usr/local/go/bin to PATH
      mkdir -pv /root/gowork/src/github.com											          # create workspace folders
      echo 'export GOPATH=/root/gowork' >> /.profile								        # update GOPATH
      echo 'export PATH=$PATH:$GOPATH/bin' >> /.profile						   	# update PATH
      . /.profile 															                    	# apply changes
    

      #==================================================================================================
      #	  3. Beego (framework)
      #==================================================================================================
      go get github.com/astaxie/beego
      go get github.com/beego/bee

