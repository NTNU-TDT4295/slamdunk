import socket
import struct
import threading
import numpy as np
from matplotlib import pyplot as plt
from matplotlib import animation

data_x = []
data_y = []
data_z = []

data_lock = threading.Lock()

class RecvThread(threading.Thread):
    def run(self):
        print('In thread')
        soc = socket.socket()
        soc.bind(('0.0.0.0', 6002))
        soc.listen()

        soc, addr = soc.accept()
        print('Connected to', addr)

        while True:
            data = []
            while len(data) < 6:
                data = soc.recv(6 - len(data))

            res = struct.unpack('<hhh', data)

            data_lock.acquire()
            data_x.append(res[0])
            data_y.append(res[1])
            data_z.append(res[2])
            data_lock.release()

thread = RecvThread()
thread.start()



plt.ion()
fig = plt.figure()
ax = fig.add_subplot(1, 1, 1)
# line1, = ax.plot(t, data_x, 'r-')

def tick(i):

    ax.clear()

    data_lock.acquire()
    x = range(len(data_x))
    ax.plot(x, data_x, 'r-')
    ax.plot(x, data_y, 'g-')
    ax.plot(x, data_z, 'b-')
    data_lock.release()

a = animation.FuncAnimation(fig, tick, interval=10)



plt.show()

while True:
    plt.draw()
    fig.canvas.draw()
    fig.canvas.flush_events()
    # line1.set_ydata(data_x)
    # line1.set_xdata(t)
    # # plt.show()
    # fig.canvas.draw()
    # fig.canvas.flush_events()
