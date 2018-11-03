# SLAMdunk

SLAMdunk is a project built to map out environments, using a custom
built PCB shield meant for a Xilinx PYNQ-Z1.

## build

Contains common recipes for building the modules of SLAMdunk
(e.g. makefiles).

## efm32

Code to run MCU on the custom PCB. Functionality includes reading
sensor data from RPLIDAR A1, BNO055, HC-SR04, and communicating over
serial protocols such as UART, I2C and SPI.

## libs

* slamcommon -- Hector SLAM algorithm and common net utilities.
* window -- library for creating X11 windows with OpenGL, and other 3D-rendering utilities.

## pcb

Schematics for the custom PCB shield.

## pynq

General utilities used for deployment to PYNQ. e.g. kernel modules for
Wi-Fi and virtual serial port on USB, and other scripts to get
started.

## pynqslam

The actual SLAMdunk implementation. All functionality related to
running the Hector SLAM algorithm on the PYNQ resides there. This
includes an FPGA implementation of an SPI slave, with double
buffering of the incoming data, as well as the entire implementation
required to run SLAM on a PYNQ. Also contains compiled bit files
needed to deploy to FPGA fabric.

## slamvis

Software running a remote machine for visualizing the output from
pynqslam.

## tools (for fools)

Utilities for testing etc., mainly used in previous iterations of the
system:

* lidar -- visualize the raw output from a LIDAR
* lidar_usb -- receive RPLIDAR output from USB
* pointcloud -- legacy program for visualizing 3D pointclouds
* simulator -- legacy program for simulating LIDAR data
* slam -- alternative implementation of pynqslam for running on non-PYNQ machine

