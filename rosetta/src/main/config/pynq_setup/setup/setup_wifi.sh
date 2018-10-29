#!/bin/bash

# Setup wifi driver
cp /home/xilinx/8723bu.ko /lib/modules/4.9.0-xilinx/kernel/drivers/
sudo insmod /lib/modules/4.9.0-xilinx/kernel/drivers/8723bu.ko

#autoload wifi-module
echo "8723bu" >> /etc/modules
