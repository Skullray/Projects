import time
import psutil
import matplotlib.pyplot as plt
import numpy as np

time = 30 #change this to record for longer sessios
npd = np.zeros(time)
npu = np.zeros(time)
plt.ion()

def main():
    old_valued = 0
    old_valueu = 0

    while True:
        new_valueu = psutil.net_io_counters().bytes_sent
        new_valued = psutil.net_io_counters().bytes_recv

        if old_valued:
            send_statd(new_valued - old_valued)
        if old_valueu:
            send_statu(new_valueu - old_valueu)

        old_valued = new_valued
        old_valueu = new_valueu
        global npd
        global npu

        plt.clf()
        down, = plt.plot(npd, label='down')
        up, = plt.plot(npu, label='up')
        plt.legend([down, up], ['Download', 'Upload'])
        plt.ylabel('Data in MB')
        plt.xlabel('Time (sec)')
        plt.pause(0.05)

        time.sleep(0.95)

def convert_to_gbit(value):
    return value/1024./1024.*8

def send_statd(value):
    global npd
    npd = np.delete(npd, 0)
    npd = np.append(npd,[convert_to_gbit(value)])
    #print(npa.shape)
    #print ("%0.3f" % convert_to_gbit(value))

def send_statu(value):
    global npu
    npu = np.delete(npu, 0)
    npu = np.append(npu,[convert_to_gbit(value)])
    #print(npa.shape)
    #print ("%0.3f" % convert_to_gbit(value))

main()
