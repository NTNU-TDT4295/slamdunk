#!/bin/bash

install /home/xilinx/slamdunk/pynq/systemd/pynqslam.service /etc/systemd/system/pynqslam.service
install /home/xilinx/slamdunk/pynq/systemd/bitfile.service /etc/systemd/system/bitfile.service
install -m 0440 /home/xilinx/slamdunk/pynq/systemd/pynqslam.sudoers /etc/sudoers.d/pynqslam
systemctl daemon-reload
systemctl enable bitfile.service
