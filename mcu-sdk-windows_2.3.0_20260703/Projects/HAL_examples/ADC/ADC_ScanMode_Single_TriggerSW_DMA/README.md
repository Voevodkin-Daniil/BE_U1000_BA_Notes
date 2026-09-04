# Description

ADC usage example in scan mode.
It performs single conversion on 8 ADC channels every 1s.
The conversion is started by software.
The end of conversion is detected using the DMA interrupt.
The conversion values are transferred by DMA.
The result is printed to the debug UART interface.

# Hardware configuration

ADC0 (VIN0-VIN7)
