# Snort IDS VNF

VNF uses SNORT as DPI (Deep Packet Inspector).
One interface monitors all the traffic.
The other interface is used to contact an (Ryu) Openflow Controller to install new Openflow entries in the switches, 
depending on the Snort rules config.

Snort writes the alerts defined in the rules to a socket file (in the same folder as the log files).
A second process in the VNF (`pigrelay.py`) monitors the socket file and sends the alerts to a 
Ryu openflow controller via a network socket.


Based on this exmaple: http://ryu.readthedocs.io/en/latest/snort_integrate.html

### Configuration

```
            +-----------+
input       |           |  output (alerts to Ryu)
+----------->   Snort   +--------->
            |           |
            +-----------+


```

### Entry point:

```
./start.sh
```

### Snort logs

```
/tmp
```
