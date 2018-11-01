#!/bin/bash

# setup wifi connection
echo "network={
    ssid=\"SLAMDUNK\"
    psk=\"xxx\"
    proto=RSN
    key_mgmt=WPA-PSK
    pairwise=CCMP
    auth_alg=OPEN
}" > /etc/wpa_supplicant/wpa_supplicant.conf

# setup network interfaces
echo "# The loopback network interface
auto lo
iface lo inet loopback

auto eth0
iface eth0 inet static
address 192.168.2.99
netmask 255.255.255.0
#gateway 192.168.2.89 #needed for ip ether bridge

auto wlx74da38930a18
allow-hotplug wlx74da38930a18
iface wlx74da38930a18 inet dhcp
wpa-conf /etc/wpa_supplicant/wpa_supplicant.conf" > /etc/network/interfaces

# Hardcode NTNUs dns-servers with google backup
# this option can be used if you want to directly
# connect the pynq to the internet via ethernet

# sudo chattr -i /etc/resolv.conf
# echo "nameserver 129.241.0.200
# nameserver 129.241.0.201
# nameserver 8.8.8.8" >> /etc/resolv.conf
# sudo chattr +i /etc/resolv.conf

# restart networking
sudo systemctl restart networking 2>/dev/null

# cleanup
rm /home/xilinx/setup_net.sh
