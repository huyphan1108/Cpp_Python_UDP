import socket
import datetime

def udp_server():
    server_ip = "192.168.56.1"  # IP address to bind to
    server_port = 8888      # Port to bind to

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind((server_ip, server_port))

    print(f"Listening on {server_ip}:{server_port}")

    while True:
        data, addr = sock.recvfrom(1024)
        print(f"Received message: {data.decode('utf-8')} at {datetime.datetime.now()}")

if __name__ == "__main__":
    udp_server()