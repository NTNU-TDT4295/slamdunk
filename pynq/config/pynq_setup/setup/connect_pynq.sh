#!/bin/bash

# running this script allows you to ssh to the pynq without it having a static IP. This is given you have a dynamic IPv6 yourself.

# pass it an argument for it to return just the ip address

interface=$(ip link | awk -F: '$0 !~ "lo|vir|wl|^[^0-9]"{print $2;getline}' | sed 's/ //g')

ipv6=$(ip addr show ${interface} | grep inet6)
if ! [[ $ipv6 ]]; then
    echo You need an IPv6 address on interface ${interface}
    exit 1
fi

ip=$(ping6 -c 2 ff02::1%${interface} | grep DUP | awk '{print $4}')
if [ $ip ]; then
    if [ $# -gt 0 ]; then # use any argument to just get the address
        echo $ip
        exit 0
    else
        ssh xilinx@$ip
    fi
else
    echo Not connected
    exit 1
fi
