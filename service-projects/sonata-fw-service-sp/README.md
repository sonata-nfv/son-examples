# Firewall service 

This is the service that contains only the vFW

Service chain: 

```
         +----------+    
in       |          |
+--------> Firewall +------------>
         |          |    
         +----------+    
```

## Firewall VNF

The firewall VNF consists of two parts (both installed in one Docker container):

1. Open vSwitch-based data plane
2. Ryu controller

