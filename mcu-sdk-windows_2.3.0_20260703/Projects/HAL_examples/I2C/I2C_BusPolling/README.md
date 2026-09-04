# Description

Scanning the I2C bus for devices.
It uses two I2C interfaces on a single board.
I2C1 master device operates in polling mode.
I2C2 slave device operates in interrupt mode.
The I2C1 master, starting from address 0x08, sends a command to read one byte.  When an ACK bit is received, it is assumed that the device is present on the bus. When receiving a NACK, it is assumed that the device at the specified address is missing from the bus. Next, the address is incremented and the procedure is repeated to address 0x77.
The I2C2 slave receives a byte read command in the interrupt routine and sends 0x00 in response.
The debug information is printed to UART.

## Hardware configuration

I2C1 (PORT0 pins 14,15) - I2C master
I2C2 (PORT1 pins 8,9) - I2C slave

## Settings

I2C1:
- Mode: master
- Speed: fast mode (400 kbit/s)

I2C2:
- Mode: slave
- Address: 0x55
- Speed: 400 kbit/s
