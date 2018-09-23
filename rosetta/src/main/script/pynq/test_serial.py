import serial
import socket
import time
import struct

remote_ip_addr = '193.35.55.93'
remote_port = 6001
retry_interval = 3

def connect():
    new_socket = socket.socket()
    while True:
        try:
            new_socket.connect((remote_ip_addr, remote_port))
            break
        except ConnectionRefusedError:
            print('Could not connect to %s, retrying in %d seconds' % (remote_ip_addr, retry_interval))
            time.sleep(retry_interval)
    return new_socket

def reflect(serial_port, socket):
    while True:
        recv = serial_port.read(8)
        try:
            socket.sendall(recv)
            print(recv)
        except (BrokenPipeError, ConnectionResetError):
            return

def main():
    serial_port = serial.Serial('/dev/ttyUSB0', baudrate=115200)
    while True:
        socket = connect()
        reflect(serial_port, socket)

if __name__ == '__main__':
    main()
