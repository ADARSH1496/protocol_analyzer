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
ACCEL_ADDR = 0x18   # Real NXP onboard FXLS8974CFR3 accelerometer
TARGET_ADDR = 0x7E  # Emulated Slave / Fault Injection target

while True:
    try:
        led.toggle()
        # 1. Talk to the REAL onboard Accelerometer (0x18) - Write WHO_AM_I register address (0x13)
        i2c.writeto(ACCEL_ADDR, bytearray([0x13]))
        print(f"[PICO -> ACCEL 0x18] Wrote register 0x13")
        
        time.sleep(1)

        # 2. Master WRITE to 0x7E
        i2c.writeto(TARGET_ADDR, payload)
        print(f"[PICO -> EMUL  0x7E] Sent {[hex(b) for b in payload]}")
        
        time.sleep(1)
        
        # 3. Master READ from 0x7E (Fault Injection test)
        rx_data = i2c.readfrom(TARGET_ADDR, 2)
        print(f"[PICO <- EMUL  0x7E] Read {[hex(b) for b in rx_data]}")
        
    except OSError as e:
        print(f"[PICO ERROR] Bus transfer failed ({e})")
    
    time.sleep(2)