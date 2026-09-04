# Description

#### Core_1 GPIO usage example.  
* The firmware code execution starts on the Core 0. Core 1 is disabled
* Core 0 configures the LED GPIO pin as output
* Core 0 prepares the Core 1 for code execution and starts the Core 1
* Core 0 starts the infinite loop execution
* Core 1 toggles the LED periodically

> Note 1  
> There is no any additional code (like what is contained in the startup files) for the Core 1.  

> Note 2  
> Since there is no Core 1 specific instructions, the whole project built for the Core 0's architecture and ABI. In other cases it is necessary to build Core 1 firmware separately.

Minimalistic example without clock configuration.  
MCU runs at the default clock config: 25 MHz XTAL, PLL not configured.  
BSP functions used.  
