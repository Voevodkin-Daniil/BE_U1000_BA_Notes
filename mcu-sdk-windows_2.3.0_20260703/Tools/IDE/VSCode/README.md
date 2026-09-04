# Visual Studio Code with MCU_SDK
## 1. Description
To work with MCU projects, you can use Visual Studio Code editor.  
`.vscode` project template has the following features:
#### Tasks:
* "Setup USB" (Administrator rights are required)
  * Windows: Run Zadig (The user installs the WinUSB driver manualy).
  * Linux: Create a udev rules for jtag debuggers (EVU-BA FTDI and BE-U1000 Core2)
* "Update settings" - Update the `settings.json` file, if the Makefile has been changed. (This will run automatically after starting the "Build" or "Rebuild" tasks)
* "Clean" - Clean up current project
* "Erase eFlash" - Complete erasure of the `eFlash` memory
* "Erase QSPI0 Flash" - Complete erasure of the `QSPI0` Flash memory
* "Erase QSPI1 Flash" - Complete erasure of the `QSPI1` Flash memory
* "Build" - Build current project
* "Rebuild" - Clean up and Build current project
* "Load" - Load the assembled firmware of the current project in MCU memory (See [MEM_REG_ROM](../../build/Makefile_template) parameter to specify the desired memory region)

#### Debug mode features:
* OpenOCD debug server start/stop automatic control.
* Software/harsware breakpoint automatic selection (only 3 hardware breakpoints are available).
* Automatic XIP mode setup in case of debugging `QSPIx` Flash memory located firmware
* Ability to connect to all MCU Cores

***
## 2. Basic steps
### 2.1 Environment preparation
2.1.1 Download and prepare [MCU SDK](https://mcu.baikalelectronics.ru/docs/developers/).  
2.1.2 Download and install [Visual Studio Code](https://code.visualstudio.com/Download).  

### 2.2 Project configuration
2.2.1 Use [template](../../../Projects/_template), one of available [examples](../../../Projects) or create your own project from scratch.  
>***
>**NOTE**  
>Do not use spaces in the project path.
>***
2.2.2 Set the desired build parameters in the `Makefile` file.  
>***
>**NOTE**  
>It is required to set the *SDK* peth (see 2.1.1) in `SDK_DIR` variable at least.
>***
2.2.3 Copy the `.vscode` template directory into your project working directory.  
2.2.4 Open the project in Visual Studio Code.  

### 2.3 Editor configuration
2.3.1 Install recommended extensions.
2.3.2 [*Optional*] Run the `Update settings` task to synchronize the Visual Studio Code project settings (`settings.json` file) with the Makefile-based SDK build system. `Update settings` task will be run automatically before `Build` and `Rebuild` tasks.
2.3.3 [*Optional*] Run the `Setup USB` task to configure the USB-JTAG debugger (A single launch is enough).    

### 2.4 Build frimware
Build your project and get the firmware files using the `Build` or `Rebuild` tasks.  
`Rebuild` task is equivalent to running `Clean` and `Build` tasks.

### 2.5 Load firmware
Use the `Load` task to write the firmware binary data to the MCU memory.  
>***
>**NOTE**  
>If `MEM_REG_ROM` is equal to `EFLASH`, `QSPI0`, or `QSPI1`, the corresponding memory region will be completely erased before firmware writing.
>***

### 2.6 Debug
2.6.1 [Optional] Set breakpoint(s).  
2.6.2 Start the Debug session. See [Visual Studion Code documentation](https://code.visualstudio.com/docs/debugtest/debugging) for more info about debug actions (watch variables, print local/global variables, print GPR, step-by-step execution, etc.).  
>***
>**NOTE**  
>By default, the JTAG debugger `evu-ba_ftdi` is used. If you want to use a different one, replace the `OOCD.CONFIG.DEBUGGER` parameter in the `.vscode/settings.json` file.
>***

You can also select the debugging configuration in the Activity Bar "Run and Debug":
* `Debug (CORE_0)` - Connect to the `Core 0` and start execution from the beginning of the `MEM_REG_ROM`
* `Connecn CORE_1` - Connect to the `Core 1` and halt
* `Connecn CORE_2` - Connect to the `Core 2` and halt
* `Multi-CPU: All Cores` - Connect all the Cores of the MCU, `Core 0` starts execution from the beginning of the `MEM_REG_ROM` region, `Core 1` and `Core 2` are halted
* `Multi-CPU: CORE_0 & CORE_1` - Connect to the `Core 0` and `Core 1` of the MCU, `Core 0` start execution from the beginning of the `MEM_REG_ROM` region, `Core 1` are halted

***
>### ATTENTION
> Tasks `Load`, `Erase eFlash`, `Erase QSPI0 Flash`, `Erase QSPI1 Flash` and Activity `Run and Debug` use an OpenOCD connection to a MCU via JTAG. Connection failed if you see the following error essages:
> ```sh
> Error: JTAG scan chain interrogation failed: all zeroes
> Error: Check JTAG interface, timings, target power, etc.
> Error: Trying to use configured scan chain anyway...
> Error: riscv.cpu2: IR capture error; saw 0x00 not 0x01
> Error: [CORE_0] Could not read dtmcontrol. Check JTAG connectivity/board power.
> Error: [CORE_0] Examination failed
> Error: Target not examined yet
>
> Error: [CORE_0] Unsupported DTM version: -1
> Error: [CORE_0] Could not identify target type.
> Error: [CORE_1] Unsupported DTM version: -1
> Error: [CORE_1] Could not identify target type.
> Error: [CORE_2] Unsupported DTM version: -1
> Error: [CORE_2] Could not identify target type.
> ```
> If connection failed, restart the task or the debug session.
