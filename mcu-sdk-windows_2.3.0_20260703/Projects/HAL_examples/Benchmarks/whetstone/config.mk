# Select board (EVU_BA_1_2, EVU_BA_2_0, EVU_BA_2_1, EVU_BA_2_3, EVU_BA_2_5, EVU_LI_2_0, EVU_LI_2_1)
BOARD = EVU_BA_2_5

# SDK
SDK_DIR ?= $(realpath $(CURDIR)/../../../../..)

# Select core run mode (RUN_CORE0, RUN_CORE01_PAR)
RUN_MODE ?= RUN_CORE0

CLK_CCLK ?= 200

OPT ?= fast

# Target memory (TCMA, EFLASH, QSPI1)
MEM_REG_ROM ?= EFLASH

# Math library used under math.h (STANDARD, IN_ROM)
MATH_VER ?= STANDARD

BUILD_TYPE ?= debug

# Whetstone parameter xtra: Number of iterations (100 * xtra)
PARAM_XTRA ?= 200

# Pass the optimization level to application
ifeq ($(OPT), s)
	CDEFS += -DOPT=4
else ifeq ($(OPT), fast)
	CDEFS += -DOPT=5
else
	CDEFS += -DOPT=$(OPT)
endif

ifeq ($(MEM_REG_ROM), TCMA)
	CDEFS += -DMEM_REG_ROM=0
else ifeq ($(MEM_REG_ROM), EFLASH)
	CDEFS += -DMEM_REG_ROM=1
else ifeq ($(MEM_REG_ROM), QSPI1)
	CDEFS += -DMEM_REG_ROM=2
endif

# Pass the used math library to the application
CDEFS += -DMATH_VER=$(MATH_VER)

# System parameter
ifeq ($(RUN_MODE),RUN_CORE01_PAR)
    CDEFS               += -DRUN_CORE01_PAR
else ifeq ($(RUN_MODE),RUN_CORE1)
	CDEFS               += -DRUN_CORE1
endif

CDEFS += -DXTRA=$(PARAM_XTRA)

# Path
SOURCE_COMMON_DIR = $(realpath $(CURDIR)/..)
DRIVERS_SRC_DIR = $(SDK_DIR)/Drivers/HAL/Src

#configs to make performance better - locating RAM on TCMB for faster access than default SRAM && locating constants to RAM too.
MEM_REG_RAM = TCMB
RODATA_IN_RAM = 1

# Including CFLAGS for better performance
CFLAGS += -funroll-all-loops
CFLAGS += -finline-functions
CFLAGS += -finline-limit=1000
CFLAGS += -falign-functions=4
CFLAGS += -falign-jumps=4
CFLAGS += -falign-loops=4
CFLAGS += --param max-inline-insns-auto=20
CFLAGS += -fsingle-precision-constant

# Including printf() wrapper to all files
CDEFS += -include $(SOURCE_COMMON_DIR)/wrap_printf.h
CDEFS += -DMUTEX_ADDR=0x40028000
CDEFS += -DCLK_CCLK=$(CLK_CCLK)

CSRC += $(SOURCE_COMMON_DIR)/mutex.c
CSRC += $(SOURCE_COMMON_DIR)/clk.c
CSRC += $(SOURCE_COMMON_DIR)/whetstone.c
CSRC += $(SOURCE_COMMON_DIR)/syscalls.c

CSRC += $(DRIVERS_SRC_DIR)/bmcu_uart.c
CSRC += $(DRIVERS_SRC_DIR)/bmcu_eflash.c

CDEFS_OVERRIDE = $(CDEFS) "-DCFLAGS=$(CFLAGS)"
