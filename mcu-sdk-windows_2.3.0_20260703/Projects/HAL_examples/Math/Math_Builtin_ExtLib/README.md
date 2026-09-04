# Description

Demonstrates math functions usage from built-in and external libs.
Due to underlying architecture the usage differs on different cores.
A function set is calculated on CORE0 and CORE1 one by one.
The output is printed to debug UART interface.

CORE0:

Built-in math lib is used, no P-extention.

CORE1:

Built-in DSP lib is used, P-extension is used.

NOTE:

Built-in libs are not complete.
Math functions that are not implemented in built-in lib taken from standard lib AUTOMATICALLY.
For DSP functions, built-in and external lib have different preffixes (dsp_ and bear_) and can be used at the same time.

## Memory map

CORE0:

EFLASH: 0xA0000000 - 0xA001FFFF
SRAM:   0x70000000 - 0x70003FFF

CORE1:

EFLASH: 0xA0020000 - 0xA003FBFF
SRAM:   0x70004000 - 0x70007FFF
