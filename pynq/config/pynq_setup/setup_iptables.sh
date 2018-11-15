#!/bin/bash
# this script sets up iptables on your host computer and allows connecting the
# pynq to the internet when connected to your ethernet port.
# replace wlp3s0 and enp0s31f6 with your wifi and ethernet interface respectively.
sudo iptables -t nat -A POSTROUTING -o wlp3s0 -s 192.168.2.0/24 -j MASQUERADE
sudo iptables -I FORWARD -o enp0s31f6 -s 192.168.2.0/24 -j ACCEPT
sudo iptables -I INPUT -s 192.168.2.0/24 -j ACCEPT
sudo sysctl -w net.ipv4.ip_forward=1
