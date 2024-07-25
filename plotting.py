import sqlite3
import os.path
import numpy as np
import matplotlib.pyplot as plt
from datetime import datetime

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
db_path = os.path.join(BASE_DIR, "testing1.db")
con = sqlite3.connect(db_path)
cur = con.cursor()


def delay_plot():
    cur.execute('SELECT * FROM DelayTest')
    data = cur.fetchall()

    sent_times = [row[1] for row in data]
    received_times = [row[2] for row in data]
    ble_sent = [row[3] for row in data]
    ble_received = [row[4] for row in data]

    # Convert the time strings to datetime objects
    sent_times_dt = [datetime.strptime(time, '%S.%f') for time in sent_times]
    received_times_dt = [datetime.strptime(time, '%S.%f') for time in received_times]

    # x-axis label pattern
    x_labels = ['right', 'left', 'front', 'back', 'off'] * (len(sent_times) // 5 + 1)
    x_labels = x_labels[:len(sent_times)]

    # index for the x-axis
    x_ind = np.arange(len(sent_times))
    # print(x_labels)

    # Plot Sent time and Received time for UDP
    plt.figure(figsize=(12, 6))
    plt.plot(x_ind, sent_times_dt, label='Sent Time')
    plt.plot(x_ind, received_times_dt, label='Received Time', linestyle='--')
    plt.xticks(ticks=x_ind, labels=x_labels, rotation=45)
    plt.xlabel('Data')
    plt.ylabel('Time')
    plt.title('UDP Sent Time vs. Received Time')
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.show()

    plt.figure(figsize=(12, 6))
    plt.plot(x_ind, ble_sent, label='BLE Sent')
    plt.plot(x_ind, ble_received, label='BLE Received', linestyle='--')
    plt.xticks(ticks=x_ind, labels=x_labels, rotation=45)
    plt.xlabel('Data')
    plt.ylabel('Value')
    plt.title('BLE Sent vs BLE Received')
    plt.legend()
    plt.grid(True)
    plt.show()


delay_plot()