import serial
import socket

port = serial.Serial("/dev/ttyUSB0", baudrate=115200)
socket = socket.socket()
socket.connect(('193.35.55.93', 6001))

while True:
    recv = port.read(8)
    socket.sendall(recv)
    print(recv)
