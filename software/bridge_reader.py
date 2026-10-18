import serial
import sys
import time

COM_PORT = 'COM3'  
BAUD_RATE = 115200

def send_config_command(ser, mode: int, target_addr: int, fault_bytes: list[int]):
    """
    Sends 8-byte binary configuration packet to NXP board:
    [0xBB, mode (0=Analyzer, 1=Emulator), addr_7bit, B0, B1, B2, B3, 0x55]
    """
    b = (fault_bytes + [0x00, 0x00, 0x00, 0x00])[:4]
    cmd_frame = bytearray([0xBB, mode & 0xFF, target_addr & 0x7F, b[0], b[1], b[2], b[3], 0x55])
    ser.write(cmd_frame)

def run_serial_monitor():
    rx_buffer = bytearray()
    
    try:
        print(f"Attempting to connect to {COM_PORT} at {BAUD_RATE} baud...")
        with serial.Serial(COM_PORT, BAUD_RATE, timeout=0.1) as ser:
            print("Connection established. System ready.\n")
            # Test dynamic configuration: Keep Mode=0 (Analyzer range 0x08-0x7E) 
            # and inject custom fault payload [0xBA, 0xAD, 0xF0, 0x0D]
            time.sleep(0.2)
            send_config_command(ser, mode=0, target_addr=0x7E, fault_bytes=[0xBA, 0xAD, 0xF0, 0x0D])
            
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