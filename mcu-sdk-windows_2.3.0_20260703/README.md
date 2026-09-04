## Description
SDK for Baikal Electronics microcontrollers

## Structure
```
├─ BSP             : Board Support Package files
│  ├─ Components   : Development board component drivers
│  └─ <board_name> : Specific board related files
├─ Drivers         : Periphery modules drivers
│  ├─ BASIS        : Common macros, device description, source and header files
│  └─ HAL          : Drivers implementation source and header files
├─ Middlewares
│  └─ Third_Party  : Third party components: TinyUSB, FreeRTOS, etc.
├─ Projects
│  ├─ _template    : A new project template
│  └─ HAL_examples : HAL driver use examples
└─ Tools
   ├─ build        : Build system files and Makefile template
   ├─ IDE          : IDE templates: VS Code, etc.
   ├─ OpenOCD      : On-chip debugging tool
   ├─ scripts      : Service scripts (build_examples.sh, GDB config, etc.)
   ├─ svd          : System View Description files
   └─ toolchain    : GCC-based toolchain
```

## Changelog
>**2.3.0** (_2026.07.03_)
* Update
  * FreeRTOS source code restructuring
  * OpenOCD configuration scripts
  * USB device driver: bug fixes; deinit; DMA control; EP0 control; EP buffer
    processing
  * Performance benchmarks
  * Build system: bug fixes; RODATA_IN_RAM and RAM_ACCESS_OFFSET parameters
  * BSP component drivers: USB VBus control; button; LED; serial
  * Documentation: HAL drivers, HAL examples, FreeRTOS, BSP,
    build system, OpenOCD
  * HAL driver: ADC, PWMA, PWMG
  * BASIS: XIP_CMD register support; delay functions refactoring
  * SVD files
  * QSPI image build script
  * Examples:
    * FreeRTOS: unified build system; simplification
    * PWMA: Input Capture; Output Compare; PWM Output
    * PWMG: Output Compare
    * USB device: CDC, DFU, MSC
    * TIM: PWM Output; Tim Base
    * QSPI: IS25LP032D flash support
    * UART: OneBoard Com IT
    * SPI: Mater TX DMA
    * ADC
* New
  * Visual Studio Code project template 
  * USB host driver
  * FAT File System support: FatFS R0.16
  * Examples:
    * Memory benchmark
    * USB throughput benchmark
    * USB Mass Storage (device)
    * USB Device Info (host)
    * PWMA: Time Base
    * PWMG: Input Capture; PWM Output; Time Base

>**2.2.0** (_2026.04.02_)
* Toolchain: GCC updated to version 15.2
* BSP: added EVU-BA-2.3, EVU-BA-2.5 and EVU-LI-2.1 boards support;
  added TUSB320 driver
* OpenOCD configuration scripts updated
* Benchmarks: use the unified build system
* BASIS library: delay functions fixed; USB-related definitions fixed;
  added nested interrupts macros
* HAL drivers updated: USB, ADC, CRU, 
* Examples added: SPI DMA, I2C_BusPolling, ADC (read temperature),
  EFLASH_ReadNVR, TIM_PWM, CLIC_NestedInterrupts
* Examples updated: GPIO_Input, Math
* Build system updated

>**2.1.1** (_2026.01.19_)
* Toolchain (Linux): reduce system environment libraries version requirements
* Toolchain: .specs files updated 
* OpenOCD: add config file for the BE-U1000 Core 2 internal JTAG debugger
* TinyUSB updated to version 0.20.0
* Build system updated: remove redundant messages;
  add source files existance check; add CDEFS values
* Add Core 1 and Core 2 LED blink examples
* Add PWMA TimeBase example
* Benchmark: reworked Whetstone results output formatting

>**2.1.0** (_2025.12.23_)
* EVU-BA-2.1 development board support (BSP) added
* TinyUSB updated to version 0.19.0
* Toolchain updated: add printf floating point format support
* OpenOCD scripts updated
* Build system updated: add custom memory region size set
* HAL drivers and API updates: TIM, QSPI
* Examples updated: USB, GPIO, UART, FreeRTOS, Whetstone, CLINT, CAN, TIM
* BSP functions enhanced; add the user button support
* Startup files for Core2 updated
* Delay functions fixed
* SVD files updated
* Examples build script updated
* Documentation updated
* LICENSE files added
* New version format (Semantic Versioning)

>**2.0** (_2025.10.26_)
* Build system updated: simplify QSPI-based firmware building; add ROM offset
  setting; fixed build system errors
* Toolchain updates: OpenOCD platform scripts;
  OpenOCD documentation: OpenOCD eFlash and QSPI operations rework
* BASIS library updates: fixed I2C register definitions
* HAL drivers and API updates: ADC, PWMA, PWMG, TIM, SPI, QSPI, USB, GPIO, CRU,
  CANFD, DMA, eFlash, I2S, WDT, SPI, QSPI
* Examples updates: ADC, USB, TIM, CLINT, Math, Benchmarks, WDT
* Updated XIP loader and documentation
* Enhanced support for BE_U1000 MCU
* Documentation had been improved
* Coordinate names of registers between SDK and RM

>**1.4** (_2025.09.12_)
* BMCU_UM has been renamed to BE_U1000
* Build system updated: BSP support for several development boards
* BASIS library updates: **get_hart_id, BE-U1000 Core2, PWMG, PWMA, TIM, WDT,
  USB registers defs; typedef notation; 1MHz prescaler
* HAL drivers updates: BE-U1000 USB DMA support (beta); Core2 mailbox driver;
  updated Core control; TIM, PWMA, PWMG, CAN, WDT driver;
  reimplemented QE driver; fix DMA handshake functions; BE-U1000 support
* Toolchain updates: update OpenOCD version (0.11 -> 0.12);
  OpenOCD scripts update; add missed libraries (Windows) 
* Examples updates: add XIP loader example (beta); add build_examples script;
  fix build errors; update benchmarks, WDT, USB, TIM, GPIO, UART,
  CLINT_Vectored, ADC; BE-U1000 support

>**1.3** (_2025.06.30_)
* Add BMCU-UM support
* Build system: unified build system for all examples
* Toolchain update: added OpenOCD scripts
* HAL API changes: eFlash, Tim, PWMA
* HAL drivers updates: eFlash, Tim, PWMA, USB
* BASIS library changes: MCU model header for -U and -UM models
* Changed examples directory structure
* Examples updated: eFlash, Whetstone, CoreMark, FreeRTOS, CoreCTRL, UART, ADC

>**1.2** (_2025.04.30_)
* toolchain update: .specs files
* HAL API changes: eFlash, QSPI, CRU
* HAL drivers updates: eFlash, QSPI, CRU, UART
* BASIS library changes: MCU model header
* Changed examples directory organisation
* Examples updated: CLINT, eFlash, QSPI, USB DFU, USB CDC, PWMA, CoreMark,
  FreeRTOS

>**1.1** (_2025.03.16_)
* Change OpenOCD version to 0.11
* Change HAL API: CoreCtrl, CRU
* Update toolchain for Linux: Arch and ABI clarification
* Update template project (linker script, startup code and build script)
* Update BASIS library: MCU header, delay functions, 
* Update HAL drivers: USB, TIM
* Update example: USB DFU, CLIC, Benchmark, CoreCtrl, FreeRTOS
* Add example: Math_Buildin_ExtLibk, CLINT

>**1.0** (_2025.02.28_)
* Update toochain for Windows: Arch and ABI clarification
* Update BASIS library structure
* Change HAL API: TIM, PWMA, PWMG, CRU
* Add FreeRTOS example
* Add template example project
* Fix ROM math functions header file
* Fix ROM DSP function prototypes

>**1.0 RC1** (_2025.02.21_)
* Add benchmarks (CoreMark and Whetstone) in examples
* Add OpenOCD 0.12 debugging tool
* Add toolchain and OpenOCD Windows versions
* Update toolchain to GCC 14.2 with P-extension support
* Update USB stack to tinyUSB 0.18
* Update examples: USB CDC, UART, GPIO, DMA, ADC, I2C, eFlash
* Change HAL API: GPIO, TIM, UART

>**0.2** (_2025.01.17_)
* Switch toolchain to GCC 13.2 with P-extension support
* Add I2S examples
* Add DMA examples
* Add XIP control functions into CRU driver
* Add CLIC config functions
* Add FrontPort memory mapping
* Fix UART interrupt example
* Fix UART registers definitions
* Fix DMA SRC_MSIZE field definition
* Fix CLIC driver and interrupt table
* Fix WDT driver
* Fix SPI driver
* Fix QSPI driver
* Fix UART_Init function

>**0.1** (_2024.12.15_)
* Initial internal release
