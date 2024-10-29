#!/usr/bin/env python3

import threading
import serial
import time

def writerThread(ser):
    print("Writer Thread")
    count = 0
    while(True):
        ser.write("help\n".encode())
        time.sleep(0.01)
        count += 1

def readerThread(ser):
    print("Reader Thread")
    count = 0
    while(True):
        print(ser.readline(), count)
        count += 1

def main():
    ser = serial.Serial("/dev/tty.usbmodem111301")
    writer = threading.Thread(target=writerThread, args=(ser,))
    reader = threading.Thread(target=readerThread, args=(ser,))
    writer.start()
    reader.start()
    writer.join()
    reader.join()
    ser.close()


if __name__ == "__main__":
    main()
