#!/bin/bash

# script to automate network setup of the pynq
ip=$(bash connect_pynq.sh 1)

printf 'pynq password: '
read -s password

# setup and execute wifi driver settings
expect scp.exp ${ip} $password "8723bu.ko"
expect scp.exp ${ip} $password "setup_wifi.sh"
expect ssh.exp ${ip} $password "setup_wifi.sh"


# setup and execute network settings
expect scp.exp ${ip} $password "setup_net.sh"
expect ssh.exp ${ip} $password "setup_net.sh"

exit 0
