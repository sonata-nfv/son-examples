# son-emu-sap VNF

This VNF is deployed as service endpoint or SAP (Service Access Point) in son-emu (if enabled).
Every SAP in the NSD (in the field `connection points`) is deployed as a container with this image and linked to the 
specified VNF in the NSD.
It has several tools installed to generate and dump traffic to test the service in the emulator.



