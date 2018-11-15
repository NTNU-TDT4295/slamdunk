#!/bin/bash

# Setup wifi driver
mv /home/xilinx/8723bu.ko /lib/modules/4.9.0-xilinx/
if [ ! $(sudo lsmod | grep 8723bu) ]; then
    sudo insmod /lib/modules/4.9.0-xilinx/8723bu.ko
fi

# autoload wifi-module
echo "8723bu" > /etc/modules
sudo depmod -a

sudo systemctl restart networking
# cleanup
rm /home/xilinx/setup_wifi.sh
sudo ip addr flush dev eth0
sudo systemctl restart networking 2>/dev/null
sudo dhclient
