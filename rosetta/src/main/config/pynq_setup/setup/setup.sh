#!/bin/bash

# script to automate network setup of the pynq

# get ip address of the pynq, exit if not connected
ip=$(bash connect_pynq.sh 1)

if [ "$ip" = "Not connected" ];then
    echo $ip
    exit 1
fi

# get password and setup new password if needed
printf 'current pynq password: '
read -s pass
echo
read -p "Setup new password [y/n]?" -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]; then
    printf 'new pynq password: '
    read -s new_pass
    echo 'setting new password ...'
    expect ssh.exp ${ip} $pass "echo "xilinx:${new_pass}" | sudo chpasswd " > /dev/null
    pass=$new_pass
fi

printf '\nsetup and execute wifi driver settings...\n'
expect scp.exp ${ip} $pass "8723bu.ko"
expect scp.exp ${ip} $pass "setup_wifi.sh"
expect ssh.exp ${ip} $pass "bash setup_wifi.sh" > /dev/null

printf '\nsetup and execute network settings...\n'
expect scp.exp ${ip} $pass "setup_net.sh"
expect ssh.exp ${ip} $pass "bash setup_net.sh" > /dev/null
expect ssh.exp ${ip} $pass "sed -i \"s/xxx/$pass/g\" /etc/wpa_supplicant/wpa_supplicant.conf" > /dev/null

read -p "Finished setup, reboot to enact changes [y/n]?" -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]; then
    printf '\nBooting pynq...\n'
    expect ssh.exp ${ip} $pass "reboot" > /dev/null
fi

exit 0
