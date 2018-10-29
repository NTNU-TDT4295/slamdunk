#!/bin/bash

# script to automate network setup of the pynq
ip=$(bash connect_pynq.sh 1)

printf 'pynq password: '
read -s password

printf '\nsetup and execute wifi driver settings...\n'
expect scp.exp ${ip} $password "8723bu.ko"     > /dev/null
expect scp.exp ${ip} $password "setup_wifi.sh" > /dev/null
expect ssh.exp ${ip} $password "setup_wifi.sh" > /dev/null

printf '\nsetup and execute network settings...\n'
expect scp.exp ${ip} $password "setup_net.sh" >/dev/null
expect ssh.exp ${ip} $password "setup_net.sh" >/dev/null

exit 0
