# VTC service 

This is the service that contains only the vTC.

Service chain: 

```
             +-----+
in           |     |           out
+------------> vTC +------------->
             |     |
             +-----+
```


## vTC VNF

The vTC consists of two parts (both installed in one Docker container):

1. PF_RING bridge / nDPI
2. Dashboard
