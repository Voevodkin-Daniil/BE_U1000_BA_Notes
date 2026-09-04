# Description

DMA Memory to Memory transfer example.
Transfers data from a TCM buffer to another TCM buffer.
DMA controller uses Front Port memory mapping to access buffers.
On data transfer completion both buffers are compared and the result is reported to debug UART.
The transfer is repeated every 1s.

## Hardware configuration

DMA0 (Channel 0)
