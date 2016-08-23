# Snort IDS VNF

VNF uses SNORT as DPI (Deep Packet Inspector).
One interface monitors all the traffic.
The other interface is used to contact an (Ryu) Openflow Controller to install new openflow entries in the switches, depending on the Snort config.
Based on this exmaple: http://ryu.readthedocs.io/en/latest/snort_integrate.html

### Configuration

```
    +----------------------------------------+
    |                  VNF                   |
    |                                        |
    |               +-------+                |
    |               | Snort |                |
    |               +-^---|-+                |
    |                 |   |                  |
    | +----------+    |   |    +-----------+ |
+----->eth:input +----+   +---->eth:output +------->
    | +----------+             +-----------+ |
    |                                        |
    +----------------------------------------+

```

### Entry point:

```
./start.sh
```

### Snort logs

```
/snort-logs
```
