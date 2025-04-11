# Code to read serial and save to file.
# Preparation
# 1. Install pyserial: pip install pyserial
# Load program
# 1. python serial_reader.py COM#
# Ending Program
# Use Ctrl-C to end and save the file.


import serial
import argparse
from datetime import datetime

def read_and_save_to_file(serial_port, baud_rate=115200):
    # Open serial port
    ser = serial.Serial(serial_port, baud_rate, timeout=1)

    try:
        while True:
            # Generate a timestamp for the current date and time
            timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')

            # Construct the filename using the timestamp
            file_path = f'serial_data_{timestamp}.txt'

            with open(file_path, 'w') as file:
                while True:
                    # Read a line from the serial port
                    line = ser.readline().decode('utf-8').strip()

                    # Print the line to console (optional)
                    print(line)

                    # Write the line to the file
                    file.write(line + '\n')

    except KeyboardInterrupt:
        # Handle keyboard interrupt (Ctrl+C)
        pass
    finally:
        # Close the serial port
        ser.close()

if __name__ == "__main__":
    # Create a command-line argument parser
    parser = argparse.ArgumentParser(description='Read data from a serial port and save it to a file.')

    # Add command-line arguments for the serial port and file number
    parser.add_argument('serial_port', help='The serial port to read data from (e.g., COM1 or /dev/ttyUSB0)')
   
    # Parse the command-line arguments
    args = parser.parse_args()

    # Call the function to read and save data
    read_and_save_to_file(args.serial_port)
