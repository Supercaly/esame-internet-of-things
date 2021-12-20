import os
import serial

if __name__ == "__main__":
    ser = serial.Serial("/dev/cu.usbserial-0001", 115200)

    try:
        while True:
            line = ser.readline()
            try:
                print(line.decode('UTF-8').strip('\n'))
            except UnicodeDecodeError:
                print(line)
    except KeyboardInterrupt:
        ser.close()
        print("bye-bye.")
        os._exit(0)