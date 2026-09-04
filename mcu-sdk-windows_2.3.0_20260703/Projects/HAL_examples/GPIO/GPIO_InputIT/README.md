# Description

GPIO input **interrupt mode** example.  
Configure the LED pin as output.  
Configure the user button pin as input.  
Configure the **user button pin as GPIO interrupt** source.  
Configure CLIC and enable interrupts.  
Toggle the LED in GPIO ISR.

BSP not used in order to explicitly demonstrate HAL library's functions.  
Minimalistic example without clock configuration.  
MCU runs at the default clock config: 25 MHz XTAL, PLL not configured.

There are several GPIO IT configuration parameters in Makefile:
* `BTN_IT_TYPE`: controls the type of interrupt that can occur - either edge-sensitive (EDGE) or level-sensitive (LEVEL).
Example:
```sh
    make BTN_IT_TYPE=EDGE
```
* `BTN_IT_MODE`: controls the edge type of interrupt that can occur - either one edge (rising or falling depending on the `BTN_IT_POL` parameter; ONE) or both edges (BOTH).
```sh
    make BTN_IT_MODE=BOTH
```
* `BTN_IT_POL`: controls the polarity of edge or level sensitivity.
Valid values: LOW, HIGH
```sh
    make BTN_IT_POL=HIGH
```
