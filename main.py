import asyncio
import socket
from bleak import BleakClient
from datetime import datetime
import sqlite3
import os.path
import time

# effect list for esp32
# https://cdn-shop.adafruit.com/datasheets/DRV2605.pdf

eps32_r_addr = "4C:75:25:EE:CA"
esp32_l_addr = "4C:75:25:EE:C9:62"
R_HAPTIC_UUID = "cfdef092-e196-440e-938e-981f97692a78"
L_HAPTIC_UUID = "0a067d41-ceb0-4a41-a9f5-e5b805b39f13"
R_CONNECT_UUID = "fe113624-8cd7-48f0-808e-ebb4f257ebd9"
L_CONNECT_UUID = "cc719654-2db1-4c81-9047-fe8288ccd0e7"

# Bind the socket to an address and port
server_ip = "192.168.56.1"  # IP address to bind to
server_port = 8888  # Port to bind to

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((server_ip, server_port))

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
db_path = os.path.join(BASE_DIR, "testing.db")
con = sqlite3.connect(db_path)
cur = con.cursor()


async def connect_to_server(address):
    client = BleakClient(address)
    await client.connect()
    return client


async def main():
    if not eps32_r_addr or not esp32_l_addr:
        print("Could not find both devices. Exiting...")
        return

    client_r = await connect_to_server(eps32_r_addr)
    client_l = await connect_to_server(esp32_l_addr)

    try:
        if client_r.is_connected and client_l.is_connected:
            print(f"Connected to {eps32_r_addr} and {esp32_l_addr}")
            while True:
                data, addr = sock.recvfrom(1024)  # get data from cpp
                sent_time, addr = sock.recvfrom(1024)  # get sent time from cpp
                received_data = data.decode('utf-8')  # decode the data
                sent_time = sent_time.decode('utf-8')  # decode the sent_time
                # print("Sent data: ", received_data, "at", sent_time)
                # print("Received data:", received_data)
                now = datetime.now()  # get received time
                received_time = now.strftime("%S.%f")
                print("Received data:", received_data, "at", received_time)
                # print("Type of received data: ", type(data))
                # print("Type of received time: ", type(received_time))

                if received_data == "right":
                    start_time = str(time.perf_counter())
                    await client_r.write_gatt_char(R_HAPTIC_UUID, data, response=True)
                    await client_l.write_gatt_char(L_HAPTIC_UUID, data, response=True)
                    rec_time = str(time.perf_counter())
                    # delay_time = round(rect_time - start_time, 6)
                    # delay = str(delay_time)

                elif received_data == "left":
                    start_time = str(time.perf_counter())
                    await client_l.write_gatt_char(L_HAPTIC_UUID, data, response=True)
                    await client_r.write_gatt_char(R_HAPTIC_UUID, data, response=True)
                    rec_time = str(time.perf_counter())
                    # delay_time = round(rect_time - start_time, 6)
                    # delay = str(delay_time)
                elif received_data == "front":
                    start_time = str(time.perf_counter())
                    await client_r.write_gatt_char(R_HAPTIC_UUID, data, response=True)
                    await client_l.write_gatt_char(L_HAPTIC_UUID, data, response=True)
                    rec_time = str(time.perf_counter())
                    # delay_time = round(rect_time - start_time, 6)
                    # delay = str(delay_time)
                elif received_data == "back":
                    start_time = str(time.perf_counter())
                    await client_r.write_gatt_char(R_HAPTIC_UUID, data, response=True)
                    await client_l.write_gatt_char(L_HAPTIC_UUID, data, response=True)
                    rec_time = str(time.perf_counter())
                    # delay_time = round(rect_time - start_time, 6)
                    # delay = str(delay_time)
                elif received_data == "disc":
                    connected_mesg = str.encode("Disconnected")
                    await client_r.write_gatt_char(R_HAPTIC_UUID, data, response=True)
                    await client_l.write_gatt_char(L_HAPTIC_UUID, data, response=True)
                    await client_r.write_gatt_char(R_CONNECT_UUID, connected_mesg, response=True)
                    await client_l.write_gatt_char(L_CONNECT_UUID, connected_mesg, response=True)
                    break
                else:
                    start_time = str(time.perf_counter())
                    await client_r.write_gatt_char(R_HAPTIC_UUID, data, response=True)
                    await client_l.write_gatt_char(L_HAPTIC_UUID, data, response=True)
                    rec_time = str(time.perf_counter())
                    # delay_time = round(rect_time - start_time, 6)
                    # delay = str(delay_time)

                cur.execute("INSERT INTO DelayTest VALUES "
                            "('" + received_data + "','" + sent_time + "','" + received_time + "'," + start_time + "," + rec_time + ")")
                con.commit()

    except Exception as e:
        print(e)

    finally:
        await client_r.disconnect()
        await client_l.disconnect()


loop = asyncio.get_event_loop()
loop.run_until_complete(main())
