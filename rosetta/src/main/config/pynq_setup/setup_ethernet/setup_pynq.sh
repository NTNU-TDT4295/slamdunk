#!/bin/bash

# Setup interfaces
echo $1 | sudo -i -S

echo "source /etc/network/interfaces.d/*

# The loopback network interface
auto lo
iface lo inet loopback

auto eth0
iface eth0 inet static
address 192.168.2.99
netmask 255.255.255.0
gateway 192.168.2.89" > /etc/network/interfaces

# Hardcode NTNUs dns-servers with google backup
chattr -i /etc/resolv.conf
echo "nameserver 129.241.0.200
nameserver 129.241.0.201
nameserver 8.8.8.8" > /etc/resolv.conf
chattr +i /etc/resolv.conf

# restart networking
systemctl restart networking 2>/dev/null
