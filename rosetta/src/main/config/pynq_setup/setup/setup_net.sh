#!/bin/bash

# setup wifi connection
echo "network={
    ssid=\"SLAMDUNK_5G\"
    psk=\"password\"
    proto=RSN
    key_mgmt=WPA-PSK
    pairwise=CCMP
    auth_alg=OPEN
}" > /etc/wpa_supplicant/wpa_supplicant.conf

# setup network interfaces
echo "source /etc/network/interfaces.d/*

# The loopback network interface
auto lo
iface lo inet loopback

auto eth0
iface eth0 inet static
address 192.168.2.99
netmask 255.255.255.0
gateway 192.168.2.89

#allow-hotplug wlan0
auto wlx74da38930a18

allow-hotplug wlx74da38930a18
iface wlx74da38930a18 inet dhcp
wpa-conf /etc/wpa_supplicant/wpa_supplicant.conf
iface default inet dhcp" > /etc/network/interfaces

# Hardcode NTNUs dns-servers with google backup
sudo chattr -i /etc/resolv.conf
echo "nameserver 129.241.0.200
nameserver 129.241.0.201
nameserver 8.8.8.8" > /etc/resolv.conf
sudo chattr +i /etc/resolv.conf

# restart networking
sudo systemctl restart networking 2>/dev/null
