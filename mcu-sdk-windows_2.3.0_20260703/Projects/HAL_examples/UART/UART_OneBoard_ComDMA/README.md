# Description

UART interface usage example.
Reads symbols from terminal and sends it back. 
Characters are received and transmit in DMA service routine and use DMA Transfer Block Complete interrupt.
Characters are received in automatic address reload mode. Automatic address reload mode is supported only on DMA channels 2 and 3.
