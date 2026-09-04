# Description

GPIO input **EXTI interrupt mode** example.  
Configure the LED pin as output.  
Configure the user button pin as input.  
Configure the **user button pin as EXTI interrupt** source.  
Configure CLIC and enable interrupts.  
Toggle the LED in GPIO ISR.

BSP not used in order to explicitly demonstrate HAL library's functions.  
Minimalistic example without clock configuration.  
MCU runs at the default clock config: 25 MHz XTAL, PLL not configured.
