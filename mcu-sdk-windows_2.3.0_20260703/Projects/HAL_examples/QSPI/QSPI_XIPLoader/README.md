### 1. Description
QSPI XIP loader example.  
This project demonstrates MCU ability to code execution from an external QSPI flash memory IC (i.e. **XIP mode**). Some development boards are equipped with QSPI flash memory IC with XIP mode support.  
By default,  this project is configured for the EVU-BA-2.0 board, which uses the Micron MT25QL128aBA QSPI flash memory chip.  

---
### 2. bootROM, XIP loader and Application interaction notes
#### 2.1 bootROM
There is a bootloader written in the persistent MCU memory (ROM), which is called **bootROM**. bootROM execution starts right after the MCU is powered up.  
bootROM reads the state of bootstrap pins and uses them to determine the boot mode: run bootROM command line interface; boot from the TCM memory; boot from eFlash memory; **read and analyze a special data header from the external QSPI memory**; initialize USB interface with DFU and/or CDC interfaces; etc.  
More information could be found in the bootROM documentation.  
In the context of this project, it is assumed that the "QSPI" boot mode has been selected.

#### 2.2 Data header
There are many types of external QSPI flash memory ICs on the market. Unfortunately, the procedure of switching XIP mode on/off is slightly different for them. bootROM can not control itself XIP mode for all types of external QSPI flash memory. However, it is possible to copy a code segment from QSPI flash memory (as well as from the eFlash memory) to MCU TCM memory and run this code. The "XIP loader" example project represents this kind of code for switching Micron MT25QL128aBA QSPI flash memory IC to XIP mode and starting in-QSPI memory code execution.  
If the "QSPI" boot mode has been selected, bootROM reads a data structure (header) starting from the base QSPI1 memory address. This data structure determines further bootROM actions.  
The header contains the following data fields:  
| Header field                       | Field memory offset | Comment                                                |
|------------------------------------|---------------------|--------------------------------------------------------|
| "Jump over the header" instruction | **0x00**            | *Jump to the address right after the header structure* |
| Magic number                       | **0x20**            | *Defines a destination of QSPI data to be copied*      |
| Data size                          | **0x2C**            | *Size of the data to be copied from the QSPI memory*   |

The "Magic number" header's field value determines the type of destination memory region where the code from QSPI flash memory to be copied.
For example, "0x0B10D522" value makes the bootROM copy the data from QSPI flash memory to the TCMA memory, and "0x0B10DDA2" - to the TCMB region.  
The "Data size" field value determines the amount of the data to be copied.  
See more info in the bootROM documentation.

#### 2.3 XIP loader
XIP loader project is aimed to initialize the MCU QSPI interface, switch the MCU QSPI interface and QSPI flash memory in XIP mode, and start in-QSPI user's code execution.  
XIP loader project consists of several stages:
* `CLK_Init()`:   MCU clocks initialization  
**Attention**: since the MCU QSPI peripheral block uses PLL as clock source, it is not possible to re-configure PLL in XIP execution mode. PLL is enabled and configured in the XIP loader code, which has to be executed from the TCM memory.  
* `qspi_init()`:  MCU QSPI periphery initialization (pin configuration; QSPI clock configuration)
* `xip_enable()`: switch MCU QSPI in XIP mode; switch external QSPI flash memory in XIP mode
* `xip_exec()`:   start QSPI-located application code execution in XIP mode starting from predefinded address (see `QSPI_APP_OFFSET` in Makefile)
---
### 3. XIP application execution illustration

```
┌────────────┬───┴─┴─┴─┴─┴──┬───────┐       ┌─────────────┐
│ MCU        │bootstrap pins├─1┐    │       │ QSPI flash  │
│            └──────────────┘  v    │       │ memory      │
│ ┌────────────┐       ┌───────┴────┤       ├─────────────┤<0>
│ │ QSPI       ├<──2-──┤ bootROM    ├<──3───┤ Header      │
│ │ controller ├<─-┐   │            │       │ ........... │
│ └────────────┘   │   └────────────┤       ├─────────────┤<0x30>
│                  │                │   ┌───┤ XIP loader  │
│                  6   ┌────────────┤   │   │ ........... │
│                  │   │ TCMA       │   4   │ ........... │
│                  │   ├────────────┤   │   ├─────────────┤<0x30 + Header->data size>
│                  └───┤ XIP loader ├<──┘   │             │
│ ┌────────────┐       │ (copy).... │       ├─────────────┤<QSPI_APP_OFFSET>
│ │ PLL        ├<──5───┤ .......... ├───7──>┤ Application │
│ │            │       ├────────────┤       │ ........... │
│ └────────────┘       │            │       │ ........... │
│                      │            │       ├─────────────┤
│                      │            │       │             │
│                      │            │       │             │
│                      │            │       │             │
│                      └────────────┤       │             │
└───────────────────────────────────┘       └─────────────┘
```
1 - bootROM gets bootstrap pins state and determines the boot mode  
2 - bootROM configures QSPI controller  
3 - bootROM reads QSPI flash memory header and parses it  
4 - bootROM copies data block (XIP loader in this project) to TCMA memory according to the read header,
    then runs copied code  
5 - XIP loader configures the PLL and MCU clocks  
6 - XIP loader reconfigures the QSPI controller and switches it into XIP mode  
7 - XIP loader switches QSPI flash memory in XIP mode and runs Application code (predefined address; see `QSPI_APP_OFFSET`)
                                                             
### 4. Building Application to be run from QSPI memory
XIP loader works in conjunction with the Application to be run from QSPI memory because the XIP loader must take into account Application address in QSPI flash memory.
As an example, it is required to build the "GPIO_LEDBlink" project, which should be executed from QSPI flash address 0x20000.  

#### 4.1 Build XIP loader
```sh
cd $(SDK_Path)/Projects/HAL_examples/QSPI/QSPI_XIPLoader
make QSPI_APP_OFFSET=0x20000
```
In the output directory XIP loader firmware will be created (file `QSPI_XIPLoader.bin`).  
This firmware:  
- has a header with all necessary metadata (jump command, Magic number and data size)  
- is linked to be run from TCMA memory  
- will run QSPI Application starting from **0x20000** address.  

Note: `QSPI_APP_OFFSET` default value is **0x10000**.

#### 4.2 Build Application
**Attention**: since the MCU QSPI peripheral block uses PLL as a clock source, it is not possible to reconfigure PLL in XIP execution mode. PLL is enabled and configured in the XIP loader code, which has to be executed from the TCM memory. Please make sure that there is no PLL configuration in the Application project.  
```sh
cd $(SDK_Path)/Projects/HAL_examples/GPIO/GPIO_LEDBlink
make MEM_REG_ROM=QSPI1 MEM_REG_ROM_OFFSET=0x20000
```
Application firmware file (`GPIO_LEDBlink.bin`) will be created in the output directory.  
This firmware is linked to be run from QSPI flash memory address **0x20000**.  
Note: if QSPI is selected as the ROM memory region, `MEM_REG_ROM_OFFSET` default value is **0x10000**

#### 4.3 Write XIP loader and Application to the QSPI flash memory
Write XIP loader firmware (`QSPI_XIPLoader.bin`) to **0x00** address and Application firmware (`GPIO_LEDBlink.bin`) to **0x20000** address of QSPI flash memory.  
See MCU documentation for more info about the QSPI memory programming process.  
Select "QSPI" boot mode and reset MCU.  
Application from the QSPI flash memory will run in XIP mode.
