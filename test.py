import serial
import time

mega = serial.Serial('/dev/ttyUSB0', 9600, timeout=1)
uno = serial.Serial('/dev/ttyACM0', 9600, timeout=1)

data = ''
send = ''

while True:
    data = uno.readline().decode('utf-8').rstrip()
    time.sleep(1)
    print(data)
    data = data + "\n"
#     mega.write(b"data\n")
    mega.write(str(data).encode('utf-8'))
#     mega.write(send.encode('utf-8'))