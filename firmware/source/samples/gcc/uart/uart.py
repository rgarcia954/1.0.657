# ----------------------------------------------------------------------------
# Copyright (c) 2021 Semiconductor Components Industries, LLC (d/b/a
# onsemi), All Rights Reserved
#
# This code is the property of onsemi and may not be redistributed
# in any form without prior written permission from onsemi.
# The terms of use and warranty for this code are covered by contractual
# agreements between onsemi and the licensee.
#
# This is Reusable Code.
#-----------------------------------------------------------------------------
# uart.py
#-----------------------------------------------------------------------------

import sys
import threading
try:
    import serial
    import serial.tools.list_ports as serialPorts
except ImportError:
    print("Missing 'pyserial' module, enter 'y' to install it using pip or " +
          "any other key to exit. \nNote: The installation requires python " +
          "to be accessible from the system environment variables.")
    if not input() == 'y':
        sys.exit(0)
    import os
    # Install pyserial module
    os.system("python -m pip install pyserial")
    
    import serial
    import serial.tools.list_ports as serialPorts

# Initialize a serial object with empty parameters
ser = serial.Serial()

def main():
    # Obtain serial port configuration parameters from the user
    COMPort = selectPort()
    baudrate = selectBaudrate()

    # Configure the serial port
    ser.port = COMPort
    ser.baudrate = baudrate
    ser.timeout = 1

    try:
        ser.open()
    except serial. serialutil.SerialException:
        print("\nAccess denied. Ensure that the serial port is not open elsewhere and then try again.\nExiting.")
        return 0

    # Create a new thread for polling the RX line in a non-blocking fashion
    read_event = threading.Event()
    read_event.set()
    reading_thread = threading.Thread(target=readRX, daemon=True, args=(read_event,))

    print("\nType in a 21 character string (C application expects 21 character strings\nat a time) or simply press Enter to send" +
           " 'onsemi UART'.\nEnter 'q' at any point to exit.\n")

    # Start the RX polling thread
    reading_thread.start()

    # Initialize the terminal to repeatedly take input for TX
    terminal()

    # Close the RX polling thread
    read_event.clear()

    # Close the serial port before terminating the application
    ser.close()

    return 0

def selectPort():
    # Look for all available COM ports
    ports = serialPorts.comports()

    # Print the list of available COM ports
    print('\n%s COM port(s) found:\n' % str(len(ports)))
    for port in ports:
        print("#%s %s\n" % (str(ports.index(port)), port.device))

    # Save and return user selection
    COMPort = input('Enter list index or COM port name ("COMx"): ')
    if len(COMPort) > 2:
        return COMPort
    else:
        return ports[int(COMPort)].device

def selectBaudrate():
    # Return the baudrate input by the user
    return (int(input("Enter the baudrate to use: ")))

def readRX(event):
    while(event.is_set()):
        # Print the received string if the queue is at least one character long
        if ser.in_waiting > 0:
            print("Received string: " + (ser.read(ser.in_waiting)).decode() + "\n")

def terminal():
    while(1):
        TXString = input()

        # Break out of the loop and exit if the input is "q"
        if TXString == "q":
            break

        # If nothing is entered send the default string
        if (len(TXString) == 0):
            TXString = "onsemi UART"

        # Send the string after concatenating the null character to the end
        ser.write((TXString + '\0').encode())

if __name__ == '__main__':
    sys.exit( main() )