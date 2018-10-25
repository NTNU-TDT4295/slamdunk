#!/bin/bash

# script to automate network setup of the pynq

ip=$(bash connect_pynq.sh 1)

printf 'pynq password: '
read -s password

expect scp.exp ${ip} $password
expect ssh.exp ${ip} $password
exit 0
