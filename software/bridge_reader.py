import serial
import sys
import time

COM_PORT = 'COM3'  
BAUD_RATE = 115200

def run_serial_monitor():
    rx_buffer = bytearray()
    
    try:
        print(f"Attempting to connect to {COM_PORT} at {BAUD_RATE} baud...")
        with serial.Serial(COM_PORT, BAUD_RATE, timeout=0.1) as ser:
            print("Connection established. System ready.\n")
            
            while True:
                if ser.in_waiting > 0:
                    rx_buffer.extend(ser.read(ser.in_waiting))
                    
                    while len(rx_buffer) > 0:
                        sof_idx = rx_buffer.find(b'\xAA')
                        
                        if sof_idx == -1:
                            try:
                                text = rx_buffer.decode('ascii').strip()
                                if text:
                                    print(f"[SYSTEM] {text}")
                            except UnicodeDecodeError:
                                pass
                            rx_buffer.clear()
                            break
                            
                        if sof_idx > 0:
                            pre_data = rx_buffer[:sof_idx]
                            try:
                                text = pre_data.decode('ascii').strip()
                                if text:
                                    print(f"[SYSTEM] {text}")
                            except UnicodeDecodeError:
                                pass
                            rx_buffer = rx_buffer[sof_idx:]
                            
                        if len(rx_buffer) < 3:
                            break
                            
                        payload_len = rx_buffer[1]
                        frame_len = payload_len + 3
                        
                        if len(rx_buffer) < frame_len:
                            break
                            
                        if rx_buffer[frame_len - 1] == 0x55:
                            payload = rx_buffer[2 : 2 + payload_len]
                            if payload_len >= 1:
                                raw_addr_byte = payload[0]
                                addr_7bit = hex(raw_addr_byte >> 1)
                                rw_dir = "READ (EMULATOR TX)" if (raw_addr_byte & 0x01) else "WRITE (MASTER TX)"
                                data_bytes = [hex(b) for b in payload[1:]]
                                print(f"[I2C {rw_dir}] ADDR: {addr_7bit} | DATA: {data_bytes}")
                            rx_buffer = rx_buffer[frame_len:]
                        else:
                            rx_buffer.pop(0)

                time.sleep(0.01)
                        
    except serial.SerialException:
        print(f"\n[ERROR] Could not open {COM_PORT}.")
    except KeyboardInterrupt:
        print("\nUser interrupted. Exiting gracefully.")
        sys.exit(0)

if __name__ == '__main__':
    run_serial_monitor()