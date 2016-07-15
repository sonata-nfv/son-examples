# sonata-emulator-demo

Simple example service with three chained, empty VNFs.-

## Service Graph

```
ns:input --> (Iperf) --> (Firewall) --> (Tcpdump) --> ns:output
```

## VNFs

1. `sonata-empty-vnf` (Iperf)
2. `sonata-empty-vnf` (Firewall)
3. `sonata-empty-vnf` (Tcpdump)


Notice: The VNF names, like iperf, are only placeholders. All started VNFs are based on `sonata-empty-vnf` and do not provide any functionality.

