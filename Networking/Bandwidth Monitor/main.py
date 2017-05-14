import time
import psutil
import matplotlib.pyplot as plt
import numpy as np

npa = np.array([])
plt.ion()

def main():
    old_value = 0

    while True:
        new_value = psutil.net_io_counters().bytes_sent + psutil.net_io_counters().bytes_recv

        if old_value:
            send_stat(new_value - old_value)

        old_value = new_value
        global npa

        plt.plot(npa)
        plt.ylabel('Data in MB')
        plt.xlabel('Time')
        plt.pause(0.05)

        time.sleep(1)

def convert_to_gbit(value):
    return value/1024./1024.*8

def send_stat(value):
    global npa
    npa = np.append(npa,[convert_to_gbit(value)])
    print(npa)
    print ("%0.3f" % convert_to_gbit(value))

main()
