# Description

The Whetstone benchmark

Whetstone (http://www.roylongbottom.org.uk/whetstone.htm)
Open source, Floating-point performance (single precision)

The benchmark tests floating-point arithmetic operations, function calls, assignments, fixed-point operations, and branching. The most complex loop, which runs 30 to 50% of the test time, performs floating-point calculations. The results are reported in Millions of Whetstone Instructions Per Second (MWIPS).

The user can use the following features:
* Set the target board
* Set core runs mode: Single-core (Core0) or Dual-core (Core0 + Core1) modes
* Set the UART where the results from Core1 will be output
* Set the number of iterations that will executed in the whetstone
* Set a path to the SDK
* Set the optimization level
* Set the target memory of whetstone image
* Set the core clock (MHz)
* Set the QSPI clock (MHz) when target memory is external QSPI-flash
* Set the math library used under math.h
* Set the whetstone image filename

# Parameters

The user can set these *PARAMETERS* when building the project.

```sh
$ make *PARAMETERS*
```

## Default build parameters

These parameters are described in more detail below.

```sh
$ make PARAM_XTRA=200 MATH_VER=STANDARD RUN_MODE=RUN_CORE0 CLK_CCLK=200 MEM_REG_ROM=EFLASH OPT=fast
```

As a result of the build, we obtain an image file in `./output` dorectory with specific name
```sh
$ ls ./output
./output/whetstone_200-MATH_STANDARD-RUN_CORE0-CCLK_200-MEM_EFLASH-OPT_fast.bin
```

The following template is used to generate the output file name
```Makefile
IMAGE_NAME ?= ./$(TARGET)_$(PARAM_XTRA)-MATH_$(MATH_VER)-$(RUN_MODE)-CCLK_$(CLK_CCLK)-MEM_$(MEM_REG_ROM)-OPT_$(OPT).bin
```

The result is a binary whetstone image for loading into EFLASH memory. whetstone will run with 20000 iterations only on Core0 at a core frequency of 200 MHz. Implementations of mathematical functions from the standard library are used.

## BOARD

The user can set the target board
* EVU_BA_1_2
* EVU_BA_2_0
* EVU_BA_2_1
* EVU_BA_2_3
* EVU_BA_2_5 (Default)
* EVU_LI_2_0
* EVU_LI_2_1

```sh
$ make BOARD=EVU_BA_2_5
```

## RUN_MODE

The user can set core runs mode

* RUN_CORE0 (Default)

The whetstone runs only on Core0

* RUN_CORE01_PAR

The whetstone runs on Core0 and Core1 in *parallel* mode. This means that the whetstone executind on Core0 and Core1 will runs simultaneously.
> Note: To avoid corruption of printf() output when Core0 and Core1 start at the same time, a simple mutex synchrinization is used.

Technically, the simultaneous starts of cores is implemented like this:
1. The Core0 starts the Core1, and then runs whetstone
2. The Core1 runs whetstone
3. The cores are locked on a mutex variable `uart_mutex` stored in the SRAM memory region before calling the `printf()` function

```sh
$ make RUN_MODE=RUN_CORE0
```

## PARAM_XTRA

The user can set the number of iterations (100 * xtra) that will executed in the whetstone

* 200 (Default)

```sh
$ make PARAM_XTRA=200
```

## SDK_DIR

The user can set a path to the SDK

```sh
$ make SDK_DIR=/path/to/sdk/location
```

## OPT

The user can set the optimization level

* 0
* 1
* 2
* 3
* s
* fast (Default)

```sh
$ make OPT=fast
```

## MEM_REG_ROM

The user can set the target memory of whetstone image

* TCMA
* EFLASH (Default)
* QSPI1_25
* QSPI1_50

```sh
$ make MEM_REG_ROM=EFLASH
```

> Notes about QSPI images
* To run a QSPI image in XIP mode, a bootloader is used. The bootloader configures the CLK_CCLK and QSPI_CLK frequences.
* QSPI1_25 and QSPI1_50 memory targets also specifify QSPI interfase frequency of 25 MHz or 50 MHz that passed as QSPI_CLK bootloader build parameter.

## CLK_CCLK

The user can set the core clock (MHz)

* 25
* 100
* 200 (Default)

```sh
$ make CLK_CCLK=200
```

## MATH_VER

The user can set the math library used under math.h

* STANDARD (Default)
The whetstone uses math functions only standard math library

* IN_ROM
The whetstone uses a subset of standard math library function stored in ROM

```sh
$ make MATH_VER=STANDARD
```

## IMAGE_NAME

The user can set the whetstone image filename

```sh
$ make IMAGE_NAME=whetstone.bin
```

# Result (run from EFLASH)

## Build

Default build command
```sh
$ make
```

Build result
```sh
$ ls -1 ./output/*.bin
./output/whetstone_200-MATH_STANDARD-RUN_CORE0-CCLK_200-MEM_EFLASH.bin
```

> Note: To bypass the use of the template when creating an image filename, you can replace the IMAGE_NAME build parameter like this
```sh
IMAGE_NAME=whetstone.bin
```

## Output

```
Core0: 
> main()=0xa0007314
Core0: > Starting Whetstone..
Core0: Build Feb  6 2026, 16:42:11
Core0: 
Core0: CCLK_Frequency: 200000000
Core0: CCLK_1MHzPrescaler: 199
Core0: PCLK0_Frequency: 100000000
Core0: PCLK1_Frequency: 100000000
Core0: PCLK2_Frequency: 100000000
Core0: HCLK_Frequency: 100000000
Core0: TCLK_Frequency: 25000000
Core0: CANx2CLK_Frequency: 25000000
Core0: 
Core0: x 78 sin 70 cos 70 *100
Core0: x 400 sqrt 200 exp 5459 *100
Core0: x 10000 log 460 log10f 200 *100
Core0: Compiler                                    GCC14.2.0
Core0: PC model                                    SoC
Core0: CPU                                         BMCU
Core0: Clock MHz                                   200
Core0: MATH_VER                                    STANDARD
Core0: Optimization                                fast
Core0: Compiler flags                              -funroll-all-loops -finline-functions -finline-limit=1000 -falign-functions=4 -falign-jumps=4 -falign-loops=4 --param max-inline-insns-auto=20 -fsingle-precision-constant -include /home/baikal.int/d.zyuzin/tmp1/mcu-sdk/BSP/1/bsp.h -mabi=ilp32f -march=rv32imafcn_zicsr_zifencei_zba_zbb_zbc_zbkb_zbkc_zbkx_zbs_zknd_zkne_zknh_zksed_zksh_xflush_xgost -specs=be_u1000_core0.specs -mcmodel=medany -std=c1x -Ofast -ggdb3 -save-temps -ffunction-sections -fdata-sections -fno-common -Wno-implicit -Wall -Wextra -fverbose-asm -fstack-usage
Core0: #1
Core0: #2
Core0: #3
Core0: 10^9 NOPs time is 5.012, Sec
Core0: Single Precision C/C++ Whetstone Benchmark
Core0: TimeUsed for pass = 4
Core0: [xtra]                                      200
Core0: [x100]                                      100
Core0: whetstones time is 6863, mSec 
Core0: [TimeUsed*1000]                             6587
Core0: MWIPS:                                      303.586
Core0: MWIPS per MHz:                              1.517
Core0: VAX MIPS rating:                            309.658
Core0: VAX MIPS per MHz:                           1.548
Core0: [DURATION]                                  6.587
Core0: [ITERATIONS]                                20000
Core0: [xtra]                                      200
Core0: [x100]                                      100

```
