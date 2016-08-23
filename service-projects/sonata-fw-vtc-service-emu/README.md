# Firewall VTC service for son-emu (Y1 demo service)

This is the service that will be used in our Y1 demo of the emulator.

Service chain: 

```
      +----------+         +-----+
      |          |         |     |
+-----> Firewall +---------> vTC +----->
      |          |         |     |
      +----------+         +-----+

```

## Attention

The dummy gatekeeper of the emulator does not support all aspects of our descriptors (it will be replaced by WP4's orchestrator later and does not need to be feature complete). In particular it does not support chaining of multiple VDUs inside a single VNFD. Thus we split up the descriptors in a single VNFD for each running container:

VNFDs:

* VNFD1: OVS Switch (firewall data plane)
* VNFD2: Ryu (firewall controller)
* VNFD3: vTC DPI (vTC data plane based on PF_RING)
* VNFD4: vTC dashboard (vTC control) 


This means the final service looks like this:


```
        Firewall:               vTC:

      +----------+         +-----------+
      |          |         |           |
      | Ryu Ctrl |         | Dashboard |
      |          |         |           |
      +----------+         +-----------+
            |                    |
            |                    |
      +----------+         +-----------+
      |          |         |           |
+----->     OVS  +--------->    DPI    +----->
      |          |         |           |
      +----------+         +-----------+

```
