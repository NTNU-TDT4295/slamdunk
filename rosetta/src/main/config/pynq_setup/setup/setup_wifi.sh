#!/bin/bash

# Setup wifi driver
mv /home/xilinx/8723bu.ko /lib/modules/4.9.0-xilinx/
if [ ! $(sudo lsmod | grep 8723bu) ]; then
    sudo insmod /lib/modules/4.9.0-xilinx/8723bu.ko
fi

# autoload wifi-module
echo "8723bu" > /etc/modules
sudo depmod -a

# cleanup
rm /home/xilinx/setup_wifi.sh
