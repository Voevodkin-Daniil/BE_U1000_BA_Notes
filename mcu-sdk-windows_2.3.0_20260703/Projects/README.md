# Example projects
Several example projects to demonstrate some capabilities of the
microcontroller's peripherals.  
All examples, except "Benchmarks", use the unified build system based on the
'make' utility. Build system files are located in the `Tools/build` directory.  
Each example project consist of source code files (.c, .h), build script (Makefile)
and README file.  
'Makefile' build script defines some project-specific settings. For example, it
may to specify the project name, list of source code .c-files, firmware target
memory region, development board name, MCU model, etc.  
See `Tools/build/Makefile_template` file for more information.
***
### Examples build process (**UART_printf** as example):
* #### Change the current directory to the selected project's directory:
```sh
    cd <SDK_root>/Projects/HAL_examples/UART/UART_printf
```
* #### Build the selected project:
```sh
    make
```
The firmware build process log and memory consumption will be shown.
Firmware build artifacts, including the binary firmware o be loaded to the MCU
persistent memory, can be found in the `output` directory.

