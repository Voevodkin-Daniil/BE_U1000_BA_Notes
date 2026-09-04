# Description

Core 2 CLINT vectored mode interrupts usage example.
It involves 3 microcontroller cores running.

CORE0:

Waits for the CORE2 interrupt and counts triggers of this.
The number of triggers is printed to debug UART interface.
On startup it starts cores 1 and 2.

CORE1:

Works independently of other cores and provides LED indication (blink with 1s period).

CORE2:

Uses system timer to trigger CLINT timer interrupt every 1s.
On interrupt it notifies Core 0.

## Memory map

CORE0:

EFLASH: 0xA0000000 - 0xA001FFFF
SRAM:   0x70000000 - 0x70003FFF

CORE1:

EFLASH: 0xA0020000 - 0xA003FBFF
SRAM:   0x70004000 - 0x70007FFF

CORE2:

TCMA(CORE2): 0x40000000 - 0x400003FF
TCMB(CORE2): 0x40000400 - 0x700007EF

# Build

Build projects for all cores:

```sh
make
```
  
The result binary file is located in `CLINT/CLINT_Vectored/output` directory.
