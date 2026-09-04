# Description

CANFD interface usage example.
Implements communication between two CANFD interfaces on the same board using polling.

## CANFD0

CANFD0 sends a standard data message with StdID = 0x321 every 1s.
The data being sent is the current value of the core time counter (Data[0] = LSB).
Acceptance filter passes a standard data message with StdID = 0x123.
Upon receiving the message it outputs data to debug UART0 and toggles LED.

## CANFD1

CANFD1 sends a standard data message with StdID = 0x123 every 1s.
The data being sent is the current value of the core time counter (Data[0] = LSB).
Acceptance filter passes a standard data message with StdID = 0x321.
Upon receiving the message it outputs data to debug UART0.

## Settings

- Speed: 1Mbps
- Data length: 8 bytes

## Hardware configuration

CANFD0 (PORT0 pins 14,15)
CANFD1 (PORT1 pins 6,7)
