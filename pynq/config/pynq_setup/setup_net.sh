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
iface eth0 inet dhcp
#enable for ip ether bridge
    #iface eth0 inet static
    #address 192.168.2.99
    #netmask 255.255.255.0
    #gateway 192.168.2.89
    #dns-nameservers 129.241.0.200
    #dns-nameservers 129.241.0.201
    #dns-nameservers 8.8.8.8


auto wlx74da38930a18
allow-hotplug wlx74da38930a18
iface wlx74da38930a18 inet dhcp
wpa-conf /etc/wpa_supplicant/wpa_supplicant.conf" > /etc/network/interfaces

# restart networking
#sudo ip addr flush dev eth0
#sudo systemctl restart networking 2>/dev/null

# cleanup
rm /home/xilinx/setup_net.sh
