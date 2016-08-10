#! /bin/bash

# cleanup sonata sdk
function term_handler() {
   echo "stopping son-analyze" 

}

trap 'term_handler' SIGINT SIGTERM 

# keep container running
while true
do
	sleep 1
done
