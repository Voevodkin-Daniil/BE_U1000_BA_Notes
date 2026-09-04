# OpenOCD & GDB Quick Start Guide
## Prepare (Linux only; optional)
You can add `udev` rules for accessing device using OpenOCD without superuser premissions
```sh 
sudo cp <SDK_path>/Tools/OpenOCD/80-bmcu.rules /etc/udev/rules.d/
sudo udevadm control --reload-rules
sudo udevadm trigger --subsystem-match=usb
```
NOTE: "80-bmcu.rules" allows you to use the "onboard ftdi" and "Core2".

## 1 Run OpenOCD
### 1.1 Run OpenOCD with onboard JTAG debugger
For BE-U1000:
```sh
<SDK_path>/Tools/OpenOCD/bin/openocd -f <SDK_path>/Tools/OpenOCD/share/openocd/scripts/board/baikal/EVU_BA_2_5.cfg
```
For BMCU-U:
```sh
<SDK_path>/Tools/OpenOCD/bin/openocd -f <SDK_path>/Tools/OpenOCD/share/openocd/scripts/board/baikal/EVU_BA_1_2.cfg
```
### 1.2 Run OpenOCD with other JTAG debugger
By default, the evu_ba*.cfg file uses the "onboard ftdi" debugger.
You can replace it with available <debugger> configs:
* jlink-jtag.cfg for SEGGER J-Link (v9.3 and higher)
* olimex-jtag.cfg for OLIMEX-ARM-USB-OCD-H
* ft2232h-jtag.cfg for FT2232H based board
* core2-jtag.cfg for Core2 as JTAG

For BE-U1000:
```sh
<SDK_path>/Tools/OpenOCD/bin/openocd -f interface/baikal/<debugger> -f board/baikal/EVU_BA_2_5.cfg
```
For BMCU-U:
```sh
<SDK_path>/Tools/OpenOCD/bin/openocd -f interface/baikal/<debugger> -f board/baikal/EVU_BA_1_2.cfg
```

For other boards:
```sh
<SDK_path>/Tools/OpenOCD/bin/openocd [-f target/baikal/<MCU>/registers.cfg] -f interface/baikal/<debugger> -f board/baikal/generic.cfg
```
As <MCU> use:
* BE_U1000 (default choice)
* BMCU_U

## 2 Run GDB and connect to OpenOCD
### 2.1 In another terminal session:
```sh
<SDK_path>/Tools/toolchain/riscv32-none-elf/bin/riscv32-none-elf-gdb -x <SDK_path>/Tools/scripts/bmcu.gdb
```

## 3 Write firmware
If you need to flash different areas of memory, then you should do it in the following order: 
1. eFLASH
2. QSPI1
3. TCM*
### 3.1 eFlash
#### 3.1.1 Erase eFlash memory (if required):
```sh
bmcu eFlash erase   
bmcu reset
```
#### 3.1.2 Write firmware:
```sh
restore <path_to_file>.bin binary <address>
```
NOTE: eFlash starts at address 0xa0000000
### 3.2 TCM*
#### 3.2.1 Write firmware to TCM*:
```sh
restore <path_to_file>.bin binary <address>
```
NOTE: TCMA starts at address 0x40010000, TCMB at 0x40020000
### 3.3 QSPI1 flash
Writing to QSPI flash uses TCMA and TCMB as a temporary buffer. The data in TCMs memory will be corrupted.
QSPI flash is unavailable to direct write. You must use OpenOCD QSPI build-in driver for this case:
#### 3.3.1 Initialize QSPI
```sh
bmcu QSPI_clk_init
bmcu QSPI1 init
```
#### 3.3.2 Erase QSPI flash memory (if required):
```sh
bmcu QSPI1 erase
```
#### 3.3.3 Write firmware:
```sh
bmcu flash write_bank 1 <path_to_file>.bin [offset]
bmcu reset
```
NOTE: Default SDK QSPI1 offset 0x10000

## 4 Prepare to Debug (if required)
### 4.1 Load debug symbols
```sh
file <path_to_file>.elf
```
### 4.2 Prepare eFlash & QSPI flash
Before debug, firsts you must to prepare flash memory:
#### 4.2.1 eFlash
```sh
enable mem 3
```
NOTE: If you need load firmware again in same debug seession you need to use command "disable mem 3"
#### 4.2.2 QSPI flash:
```sh
bmcu QSPI_clk_init
bmcu QSPI1 init

bmcu QSPI1 XIP on
```
NOTE: If you need load firmware again please reset MCU
### 4.3 Set breakpoint
```sh
break <file:line/function_name>
```
NOTE: When debugging code from eFlash/QSPI memory, only 3 hardware breakpoints are available
### 4.4 Reset MCU
```sh
bmcu reset
```

## 5 Run firmware
### 5.1 Go to Memory
```sh
bmcu jump <MEMORY>
```
NOTE: You can use "TCMA", "TCMB", "QSPI1", "eFlash" or hex value (like 0x90010000) as MEMORY param

### 5.2 Execute
```sh
continue
```
NOTE: Please don`t use "run" command, as this will cause the MCU to reset incorrectly.
