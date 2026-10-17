from machine import Pin, I2C
import time

# Initialize Hardware I2C0 on GP0 (Pin 1 = SDA) and GP1 (Pin 2 = SCL) at 100kHz
i2c = I2C(0, scl=Pin(1), sda=Pin(0), freq=100000)

# Onboard green LED on the Pico for visual heartbeat
led = Pin(25, Pin.OUT)

TARGET_ADDR = 0x7E

# Note: The NXP hardware strips the I2C address byte during the address-match 
# interrupt and only puts the payload bytes into g_slave_buff.
# Because your current bridge_reader.py prints payload[0] as ADDR and payload[1:] 
# as DATA, we send 0x7E as the first payload byte so your terminal output matches!
payload = bytearray([0xDE, 0xAD])

print("--- Pico I2C Master Initialized ---")
print("Scanning physical I2C bus...")

devices = i2c.scan()
if devices:
    print("SUCCESS! Found I2C Slave device(s) at:", [hex(d) for d in devices])
else:
    print("WARNING: No I2C devices found! Check SDA/SCL/GND jumper wires and ensure NXP is powered.")

print("\nStarting continuous transmission loop...")
while True:
    try:
        led.toggle()
        # 1. Master WRITE to NXP Analyzer
        i2c.writeto(TARGET_ADDR, payload)
        print(f"[PICO WRITE] Sent {[hex(b) for b in payload]} -> Target {hex(TARGET_ADDR)}")
        
        time.sleep(1)
        
        # 2. Master READ from NXP Slave Emulator (Fault Injection test)
        rx_data = i2c.readfrom(TARGET_ADDR, 2)
        print(f"[PICO READ]  Received {[hex(b) for b in rx_data]} <- Target {hex(TARGET_ADDR)}")
        
    except OSError as e:
        print(f"[PICO ERROR] Bus transfer failed ({e})")
    
    time.sleep(2)