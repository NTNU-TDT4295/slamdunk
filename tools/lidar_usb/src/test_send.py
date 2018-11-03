import socket

f = open('lidarout', 'rb')
soc = socket.socket()
soc.connect(('0.0.0.0', 6000))

buf = f.read(1024)
while buf:
    soc.sendall(buf)
    buf = f.read(1024)
