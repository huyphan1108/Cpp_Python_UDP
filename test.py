import socket
from datetime import datetime
import sqlite3
import os.path

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
db_path = os.path.join(BASE_DIR, "testing1.db")
con = sqlite3.connect(db_path)
cur = con.cursor()


def udp_server():
    server_ip = "192.168.56.1"  # IP address to bind to
    server_port = 8888  # Port to bind to

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind((server_ip, server_port))

    print(f"Listening on {server_ip}:{server_port}")

    while True:
        data, addr = sock.recvfrom(1024)
        sent_time, addr = sock.recvfrom(1024)
        decoded_data = data.decode('utf-8')
        decoded_stime = sent_time.decode('utf-8')

        now = datetime.now()  # get received time
        received_time = now.strftime("%S.%f")
        print(f"Received message: {decoded_data} at {received_time}")
        print(f"Sent time: {decoded_stime}")

        cur.execute("INSERT INTO DelayTest VALUES "
                    "('" + decoded_data + "','" + decoded_stime + "','" + received_time + "')")
        con.commit()


if __name__ == "__main__":
    udp_server()
