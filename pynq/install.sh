#!/bin/bash

install /home/xilinx/slamdunk/pynq/systemd/pynqslam.service /etc/systemd/system/pynqslam.service
install -m 0440 /home/xilinx/slamdunk/pynq/systemd/pynqslam.sudoers /etc/sudoers.d/pynqslam
systemctl daemon-reload
