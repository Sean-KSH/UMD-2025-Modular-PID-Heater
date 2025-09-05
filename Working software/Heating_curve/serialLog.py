import serial
import csv
import threading
from datetime import datetime

# --- CONFIGURATION ---
port = "/dev/tty.usbmodem3301"   # <-- change this to your Arduino port
baudrate = 9600
outfile = "arduino_log.csv"

# --- SETUP SERIAL ---
ser = serial.Serial(port, baudrate, timeout=1)

# --- CSV FILE ---
csv_file = open(outfile, mode='w', newline='')
writer = csv.writer(csv_file)
writer.writerow(["timestamp", "data"])

print(f"Logging to {outfile}. Type 'S:xx' to change setpoint (e.g., S:40). Press Ctrl+C to stop.\n")

# --- Function to handle keyboard input ---
def keyboard_input():
    while True:
        try:
            cmd = input("")   # wait for user to type
            ser.write((cmd + "\n").encode("utf-8"))  # send to Arduino
        except EOFError:
            break

# Start keyboard thread
threading.Thread(target=keyboard_input, daemon=True).start()

# --- Main logging loop ---
try:
    while True:
        line = ser.readline().decode("utf-8").strip()
        if line:
            timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
            writer.writerow([timestamp, line])
            print(line)
except KeyboardInterrupt:
    print("\nLogging stopped.")
    csv_file.close()
    ser.close()
