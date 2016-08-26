#!/bin/sh
. /.profile
cd $HOME/gowork/src/vtc_dashboard/
bee run watchall > allout.txt 2>&1