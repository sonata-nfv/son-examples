# Snort IDS VNF

VNF that bridges two network interfaces of a container (L2) and forwards all traffic from the input interface to the output interface. It starts snort 2.9 to monitor all traffic on this bridge. Snort can be configured by changing the snort.conf file.

### Configuration

```
    +----------------------------------------+
    |                  VNF                   |
    |                                        |
    |               +-------+                |
    |               | Snort |                |
    |               +---^---+                |
    |                   |                    |
    | +----------+    +-+-+    +-----------+ |
+----->eth:input +---->br0+---->eth:output +------->
    | +----------+    +---+    +-----------+ |
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
