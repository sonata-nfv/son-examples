# SONATA SDK installation 

This folder holds the scripts and files to startup the SONATA SDK environment in a 
modular way.
Every SDK module is built as a docker containers and brought up via docker-compose.

##### Prerequisites
- docker
- docker-compose
- SDK modules are pre-built or can be pulled from a repository

##### SDK containers
These containers should be pulled or pre-built first on the local system:
- registry.sonata-nfv.eu:5000/son-cli
- registry.sonata-nfv.eu:5000/son-emu
- registry.sonata-nfv.eu:5000/son-catalogue
- registry.sonata-nfv.eu:5000/son-analyze

## deployment
##### Startup the containers:

`sudo docker-compose -p sonatasdk up`

##### Deploy a service on the emulator via son-cli:
- Either get a prompt into the son-cli container via:
`docker exec -it son-cli /bin/bash`
- Or execute the commands via `docker exec son-cli <command>`
 
**Example**: deploy the included test_service package
 (TODO: use son-cli push command)
```
curl -i -X POST -F package=@sonata-sdk-test-service.son http://127.0.0.1:5000/packages
curl -X POST http://127.0.0.1:5000/instantiations -d "{}"
```

##### Interactions between sdk containers
To be further implemented:
- son-catalogue should hold template of vnfd and nsd
- pull vnf, service descriptors from the son-catalogue
- create workspace, project in son-cli and package a service
- deploy the service on son-emu from son-cli using son-push
- export specific metrics from son-cli using son-monitor
- analyze certain metrics using son-analyze
- debug and fine-tune the service
- push the service to the SP


##### Shutdown the sdk:

`sudo docker-compose -p sonatasdk down -v`

### start-up scripts
Each container can have a specific startup procedure specified by the command field in `docker-compose.yml`.

For **son-emu** the topology loaded at startup is specified: 
`son-emu_test_topo.py`.

For **son-cli** there is a customized shutdown procedure in `start_son_cli.sh`.

