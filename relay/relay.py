import socket
import struct
import serial
import time

# setup serial connection to stm32
SERIAL_PORT = 'COM4' # com port of the stm32 goes here
BAUD_RATE = 115200

try:
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    print(f"Connected to STM32 on {SERIAL_PORT}")
except Exception as e:
    print(f"Could not open serial port: {e}")
    exit()

# setup UDP Connection to X-Plane 11
UDP_IP = "0.0.0.0"
UDP_PORT = 49003  

# inet for ipv4, dgram for udp
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) 
sock.bind((UDP_IP, UDP_PORT))
sock.settimeout(1.0)  # seconds

print("Reading live Altitude and Vertical Speed from X-Plane 11...")

try:
    while True:
        try:
            data, addr = sock.recvfrom(1024)
            
            if data[0:4] == b'DATA':
    
                idx = 5 # we start at byte 5 (skipping the 'DATA' header) 
                while idx < len(data):
                    category_index = struct.unpack("I", data[idx : idx + 4])[0] # [0] cus struct.unpack returs a tuple, we want an integer
                    
                    # category 20: Latitude, longitude, & altitude (to grab altitude)
                    if category_index == 20:
                        true_altitude = struct.unpack("f", data[idx + 12 : idx + 16])[0]
                        alt_str = f"A{int(true_altitude):>5}"[:6]  # 'A' + 5 characters right-justified
                        
                        ser.write(alt_str.encode('ascii')) # convert strings to byte to be sent over to the stm32
                        ser.flush()
                        print(f"Sent: {alt_str}") # print in terminal for debugging
                        
                        time.sleep(0.01) # give sometime for the LCD to move cursor

                    # category 4: Mach, VVI, G-Load (for vertical speed aka VVI)
                    elif category_index == 4:
                        v_speed = struct.unpack("f", data[idx + 12 : idx + 16])[0]
                        vs_str = f"V{int(v_speed):+5}"[:6] # 'V' + explicit sign 
                        
                        ser.write(vs_str.encode('ascii'))
                        ser.flush()
                        print(f"Sent: {vs_str}")
                        
                        time.sleep(0.01)
                    
                    idx += 36  # move to the next data group in the packet because each instrument category like 4, 20 is 36 byte
        except socket.timeout:
            continue

except serial.SerialException as e:
    print(f"Serial connection lost: {e}")            
except KeyboardInterrupt:
    print("\nStopping script.")
finally:
    sock.close()
    if 'ser' in locals() and ser.is_open:
        ser.close()