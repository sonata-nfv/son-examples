# README 


## Environment Preparation

The following actions need to be done on the host, in order for PF_RING to work on the container
The vTC uses the PF_RING kernel module and as Docker containers use para-virtualization, the module needs to be loaded on the host. 
To do so, the following script has to be execute ON THE HOST MACHINE which runs the containers:

```
sudo ./prepare_host.sh
```


## Build Docker

The container can be created with the following command (more details in the main README.md of this repository).

```
docker build -t=vtc-vnf .
```

## Run the container

This container has to be executes with the `privileged` flag set in order to be able to use the PF_RING kernel module.
This flag is automatically set if you use the container with `son-emu`.