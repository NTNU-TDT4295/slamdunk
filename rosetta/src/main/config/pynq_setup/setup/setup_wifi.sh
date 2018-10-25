#!/bin/bash

# Setup wifi driver
echo $1 | sudo -i -S

cp /home/xilinx/8723bu.ko /lib/modules/4.9.0-xilinx/kernel/drivers/
sudo insmod /lib/modules/4.9.0-xilinx/kernel/drivers/8723bu.ko
