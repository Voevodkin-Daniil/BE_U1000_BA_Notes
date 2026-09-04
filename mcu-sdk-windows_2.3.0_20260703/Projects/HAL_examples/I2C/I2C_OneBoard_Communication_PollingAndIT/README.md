# Description

I2C interface usage example.
It uses two I2C interfaces on a single board.
I2C master device operates in polling mode.
I2C slave device operates in interrupt mode.
The I2C master sends a string to the slave every 1s and receives the same amount of data.
The I2C slave receives data into a ring buffer in the interrupt routine and sends it back on read request from the I2C master.
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
