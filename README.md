# parallfetch
Fast Application Launch on Personal Computing/Communication Devices

# Install steps
1. Download kernel 4.12.9 (https://mirrors.edge.kernel.org/pub/linux/kernel/v4.x/linux-4.12.9.tar.xz)
2. Applying linux-4.12.9-pf.patch to downloaded kernel
3. Build utility
   1) make
   2) mv pfsetmode pfviewer /usr/bin
   3) sudo chmod u+s /usr/bin/pfsetmode
   4) sudo mkdir /flashfetch
   5) sudo chmod 777 /flashfetch
   
