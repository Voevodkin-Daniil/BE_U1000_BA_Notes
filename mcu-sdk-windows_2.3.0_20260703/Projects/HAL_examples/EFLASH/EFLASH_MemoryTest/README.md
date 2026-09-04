# Description

EFLASH programming example.
Performs entire EFLASH memory test.
First, it erases entire chip, then writes it with 0xA5A5A5A5 pattern word by word and reads it back.
On completion the number of words with error is reported.
The debug information is printed to UART.
The program is intended to run from TCM.
