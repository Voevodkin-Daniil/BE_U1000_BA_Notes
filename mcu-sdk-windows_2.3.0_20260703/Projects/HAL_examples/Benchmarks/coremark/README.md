# Description

The CoreMark benchmark

CoreMark (https://www.eembc.org/coremark/scores.php)
Open source, Integer performance

A synthetic integer arithmetic performance test that contains implementations of the following algorithms:
* list processing (find and sort)
* matrix manipulation (common matrix operations)
* state machine (determine if an input stream contains valid numbers)
* CRC (cyclic redundancy check)

The user can use the following features:
* Set the target board
* Set core runs mode: Single-core (Core0) or Dual-core (Core0 + Core1) modes
* Set the UART where the results from Core1 will be output
* Set the number of iterations that will executed in the coremark
* Set a path to the SDK
* Set the optimization level
* Set the target memory of coremark image
* Set the core clock (MHz)
* Set the QSPI clock (MHz) when target memory is external QSPI-flash
* Set the coremark image filename

# Parameters

The user can set these *PARAMETERS* when building the project.

```sh
$ make *PARAMETERS*
```

## Default build parameters

These parameters are described in more detail below.

```sh
$ make PARAM_ITERATIONS=10000 RUN_MODE=RUN_CORE0 CLK_CCLK=200 MEM_REG_ROM=EFLASH OPT=fast
```

As a result of the build, we obtain an image file in `./output` dorectory with specific name
```sh
$ ls ./output
./output/coremark_10000-RUN_CORE0-CCLK_200-MEM_EFLASH-OPT_fast.bin
```

The following template is used to generate the output file name
```Makefile
IMAGE_NAME ?= ./$(TARGET)_$(PARAM_ITERATIONS)-$(RUN_MODE)-CCLK_$(CLK_CCLK)-MEM_$(MEM_REG_ROM)-OPT_$(OPT).bin
```

The result is a binary coremark image for loading into EFLASH memory. Coremark will run with 10,000 iterations only on Core0 at a core frequency of 200 MHz.

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

The coremark runs only on Core0

* RUN_CORE1

The coremark runs only on Core1

* RUN_CORE01_PAR

The coremark runs on Core0 and Core1 in *parallel* mode. This means that the coremark executind on Core0 and Core1 will runs simultaneously.
> Note: To avoid corruption of printf() output when Core0 and Core1 start at the same time, a simple mutex synchrinization is used.

Technically, the simultaneous starts of cores is implemented like this:
1. The Core0 runs the Core1, and when runs whetstone
2. The Core1 runs whetstone
3. The cores are locked on a mutex variable `uart_mutex` stored in the SRAM memory region before calling the `printf()` function

```sh
$ make RUN_MODE=RUN_CORE0
```

## CORE1_CONSOLE

The user can set the UART where the results from Core1 will be output

* UART0 (Default)
* UART1

```sh
$ make CORE1_CONSOLE=UART0
```

## PARAM_ITERATIONS

The user can set the number of iterations that will executed in the coremark

* 1
* 10
* 100
* 1000
* 10000 (Default)

```sh
$ make PARAM_ITERATIONS=10000
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

The user can set the target memory of coremark image

* TCMA
* EFLASH (Default)
* QSPI1

```sh
$ make MEM_REG_ROM=EFLASH
```

> Notes about QSPI images
* To run a QSPI image in XIP mode, a bootloader is used. The bootloader configures the CLK_CCLK and QSPI_CLK frequences.
* QSPI1 memory targets also can specifify QSPI interfase frequency of 25 MHz (default) or 50 MHz that passed as QSPI_CLK bootloader build parameter (i.e. QSPI_CLK=50 ).

## CLK_CCLK

The user can set the core clock (MHz)

* 25
* 100
* 200 (Default)

```sh
$ make CLK_CCLK=200
```

## IMAGE_NAME

The user can set the coremark image filename

```sh
$ make IMAGE_NAME=coremark.bin
```

As a result of the build, we obtain an image file in `./output` directory with the name specified by the user
```sh
$ ls -1 ./output/*.bin
./output/coremark.bin
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
./output/coremark_10000-RUN_CORE0-CCLK_200-MEM_EFLASH.bin
```

> Note: To bypass the use of the template when creating an image filename, you can replace the IMAGE_NAME build parameter like this
```sh
IMAGE_NAME=coremark.bin
```

## Output

```
Core0:
> main()=0xa0007140
Core0: > Sections: _stext=0xa0000000, _srodata=0x40010000, _sdata=0x40010e84
Core0: > Starting CoreMark..
Core0: Build ...
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
Core0: CPU start tick counter: 42609850
Core0: Coremark iterate start tick counter: 42613573
Core0: Coremark iterate stop  tick counter: 57217006
Core0: 2K performance run parameters for coremark.
Core0: CoreMark Size    : 666
Core0: Total ticks      : 14603434
Core0: Core clock (MHz) : 200
Core0: Total time (secs): 14.603
Core0: Iterations/Sec   : 684.770
Core0: Coremarks/Mhz    : 3.423
Core0: Iterations       : 10000
Core0: Compiler version : GCC14.2.0
Core0: Compiler flags   : -march=rv32imafcn_zicsr_zifencei_zba_zbb_zbc_zbkb_zbkc_zbkx_zbs_zknd_zkne_zknh_zksed_zksh_xflush_xgost -mabi=ilp32f -specs=BMCU_core0.specs -Ofast -g3 -fno-common -ffunction-sections -falign-functions=16 -falign-jumps=4 -falign-loops=4 -finline-limit=1000 -fno-reg-struct-return -fno-rename-registers -fno-crossjumping -freorder-blocks-and-partition -fno-tree-loop-if-convert -fno-tree-sink -fgcse-sm -finline-functions -fdata-sections -funroll-all-loops -std=c1x -Wno-implicit -nostartfiles -mcmodel=medany -Wall -Wextra -fmessage-length=0 -static -funroll-loops -fno-strict-overflow -fvisibility=hidden
Core0: Memory location  : STACK
Core0: seedcrc          : 0xe9f5
Core0: [0]crclist       : 0xe714
Core0: [0]crcmatrix     : 0x1fd7
Core0: [0]crcstate      : 0x8e3a
Core0: [0]crcfinal      : 0x988c
Core0: Correct operation validated. See README.md for run and reporting rules.
Core0:
---------------- CoreMark CRC validation ----------------
Core0: CoreMark validation result: OK
Core0: CPU stop tick counter: 57343241
```
