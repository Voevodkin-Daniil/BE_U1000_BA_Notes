# Description

#### Core_2 GPIO usage example. 
> Core 2 firmware built in assumption that the LED is connected to the PC0 pin.  
> See `Makefile`, variable `BOARD_SUPPORTED` for the list of supported boards.  

* The firmware code execution starts on the Core 0. Core 2 is disabled
* Core 0 configures the LED pin as PIO
* Core 0 prepares the Core 2 for code execution: releases Core 2 processor complex and TCM arbitrator reset signals
* Core 0 copies the prebuilt Core 2 firmware to the TCMA memory region of the Core 2
* Core 0 starts the Core 2's code execution from the Core 2 TCMA memory base address
* Core 0 starts the infinite loop execution
* Core 2 toggles the LED periodically by alternating the corresponding PIO pin state

Core 2 firmware source code is provided in the `main.c` file.
 
> There is no any additional code (like what is contained in the startup files) for the Core 2.

Minimalistic example without clock configuration. MCU runs at the default clock config.
