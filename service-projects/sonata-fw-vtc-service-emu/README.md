# Firewall VTC service for son-emu (Y1 demo service)

This is the service that will be used in our Y1 demo of the emulator.

Service chain: 

```
         +----------+    +- ---+
in       |          |    |     |           out
+--------> Firewall +----> vTC +------------->
         |          |    |     |
         +----------+    +-----+
```

## Firewall VNF

The firewall VNF consists of two parts (both installed in one Docker container):

1. Open vSwitch-based data plane
2. Ryu controller

## vTC VNF

The vTC consists of two parts (both installed in one Docker container):

1. PF_RING bridge / nDPI
2. Dashboard




