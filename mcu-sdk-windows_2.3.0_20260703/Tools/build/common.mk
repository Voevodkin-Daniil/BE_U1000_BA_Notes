## The main makefile for building firmware bases on Baikal MCU SDK
## Supposed to be included into the project's makefile
## Execute "make help" to see a list of available targets and variables
#-... -- -.-. ..- .-.-.- ... -.. -.- .-.-.- ..--- ----- ..--- ..... -...- -.--.-
# OS dependent variables
ifeq ($(OS),Windows_NT)# Windows_NT on XP, 2000, 7, Vista, 10...
    CURRENT_OS := Windows## Operation system name
else
    UNAME_S := $(shell uname)
    ifeq ($(UNAME_S),Linux)
        CURRENT_OS := Linux
    else
        $(error Unsupported OS)
    endif
endif# $(OS)

ifeq ($(CURRENT_OS),Windows)
    PROC_NUM ?= $(NUMBER_OF_PROCESSORS)## Number of 'make' parallel processes
else ifeq ($(CURRENT_OS),Linux)
    PROC_NUM ?= $(shell grep -c ^processor /proc/cpuinfo)
endif# $(CURRENT_OS)

#-... -- -.-. ..- .-.-.- ... -.. -.- .-.-.- ..--- ----- ..--- ..... -...- -.--.-
# Set "make" itself environment variables
MAKEFLAGS += --jobs=$(PROC_NUM)## "make" utility environment variables
MAKEFLAGS += --no-builtin-rules
MAKEFLAGS += --no-builtin-variables
MAKEFLAGS += --warn-undefined-variables
MAKEFLAGS += --silent
MAKEFLAGS += --no-print-directory

SHELL := bash## System shell used while building
.SHELLFLAGS := -eu -o pipefail -c# System shell configuration flags

#-... -- -.-. ..- .-.-.- ... -.. -.- .-.-.- ..--- ----- ..--- ..... -...- -.--.-
# Incude makefile with some additional variables and functions
include $(SDK_DIR)/Tools/build/service.mk

#-... -- -.-. ..- .-.-.- ... -.. -.- .-.-.- ..--- ----- ..--- ..... -...- -.--.-
# Build process service variables
# Project name. Used as the firmware file name
PRJ_DIR      := $(CURDIR)## Project working directory
BUILD_TYPE   ?= debug## Firmware build mode (debug, release, etc.)
BUILD_DIR    ?= $(PRJ_DIR)/output## Build artifacts output directory name
OUT_DIR      ?= $(BUILD_DIR)/$(BUILD_TYPE)## Build result files directory name
OBJ_DIR      ?= $(BUILD_DIR)/$(BUILD_TYPE)/obj## Intermediate files
PRJ_NAME     ?= $(shell basename $(PRJ_DIR))## The project name
SDK_DIR      ?= $(realpath $(CURDIR)/../..)## SDK location path
BSP_ROOT_DIR ?= $(SDK_DIR)/BSP## Board Support Package path
TC_DIR       ?= $(SDK_DIR)/Tools/toolchain/riscv32-none-elf## RISC-V toolchain location
TC_PREFIX    ?= $(TC_DIR)/bin/riscv32-none-elf-
V            ?= 0## Verbosity level of command execution

CMODEL       ?= medany
LDLIBS       += -lc -lm -lgcc## Required libraries (-lm, -lgcc, -lc, etc.)
Q            := @

# Set verbosity level for targets command execution output
ifneq ($(V),0)
    Q :=
    MAKEFLAGS += --no-silent
endif# $(V)

GOAL_NAME_CLEAN := clean

#-... -- -.-. ..- .-.-.- ... -.. -.- .-.-.- ..--- ----- ..--- ..... -...- -.--.-
# Use BSP only if the valid BOARD value was specified
BOARD ?= ## Development board name (EVU_BA_2_5, EVU_LI_2_1, etc.)
ifdef BOARD
    BOARD := $(findstring $(BOARD),$(BOARD_VALID))
    ifneq ($(BOARD),)
        BSP_DIR := $(BSP_ROOT_DIR)/$(BOARD)## BSP location
        include $(BSP_DIR)/bsp.mk

    else
        $(error Invalid BOARD was specified)
    endif# $(BOARD)
endif# BOARD

# Define the target MCU model
MCU_MODEL ?= $(MCU_MODEL_DEFAULT)## MCU model (BMCU_U, BE-U1000, etc.)
ifdef MCU_MODEL
    MCU_MODEL := $(findstring $(MCU_MODEL),$(MCU_MODEL_VALID))
    ifeq ($(MCU_MODEL),)
        $(error Invalid MCU_MODEL was specified: $(MCU_MODEL)$n \
                Select one of next values: $(MCU_MODEL_VALID))
    endif# $(MCU_MODEL)
endif# MCU_MODEL

# Define the target MCU core number
CORE_NUM ?= $(CORE_NUM_DEFAULT)## MCU target core number (CORE_0, CORE_1, etc.)
ifdef CORE_NUM
    CORE_NUM := $(findstring $(CORE_NUM),$(CORE_NUM_VALID))
    ifeq ($(CORE_NUM),)
        $(error Invalid CORE_NUM was specified: $(CORE_NUM)$n \
                Select one of next values: $(CORE_NUM_VALID))
    endif# $(CORE_NUM)
endif# CORE_NUM

# Select the version of math library to be used
MATH_VER ?= $(MATH_VER_DEFAULT)## Version of the math library to be used (STANARD, IN_ROM)
ifdef MATH_VER
    MATH_VER := $(findstring $(MATH_VER),$(MATH_VER_VALID))
    ifeq ($(MATH_VER),)
        $(error Invalid MATH_VER was specified: $(MATH_VER)$n \
                Select one of next values: $(MATH_VER_VALID))
    endif#$(MATH_VER)
endif# MATH_VER

#-... -- -.-. ..- .-.-.- ... -.. -.- .-.-.- ..--- ----- ..--- ..... -...- -.--.-
# Core number dependent configuration and libraries support options

# Enable/disable FPU support by ISA
FLOAT_POINT_MODE ?= $(FLOAT_POINT_MODE_DEFAULT)## FPU support by ISA (HARD, SOFT)
F_EXTENSION :=
ifdef FLOAT_POINT_MODE
    FLOAT_POINT_MODE := $(findstring $(FLOAT_POINT_MODE),$(FLOAT_POINT_MODE_VALID))
    ifeq ($(FLOAT_POINT_MODE),)
        $(error Invalid FLOAT_POINT_MODE was specified: $(FLOAT_POINT_MODE)$n \
                Select one of next values: $(FLOAT_POINT_MODE_VALID))
    endif# $(FLOAT_POINT_MODE)
    ifeq ($(FLOAT_POINT_MODE),HARD)
        F_EXTENSION := f
    endif# $(FLOAT_POINT_MODE),HARD
endif# $(FLOAT_POINT_MODE)

ifeq ($(CORE_NUM),CORE_0)# BR350X
    ABI                 ?= ilp32$(F_EXTENSION)
    ARCH                ?= rv32ima$(F_EXTENSION)cn_zicsr_zifencei_zba_zbb_zbc_zbkb_zbkc_zbkx_zbs_zknd_zkne_zknh_zksed_zksh_xflush_xgost
    ifeq ($(MCU_MODEL),BMCU_U)
        SPECS        := bmcu_u_core0.specs
    else
        SPECS        := be_u1000_core0.specs
    endif
    CDEFS               += -DCORE_BR350S1
    CDEFS               += -D_RV32_C0_
    CDEFS               += -D__BMCU_ROM1_DSP_H  # Blocks including _bmcu_rom1_dsp.h to core without this lib
    STARTUP_SUFFIX      := c0_c1
    MEM_REGS_ROM_VALID  := TCMA TCMB EFLASH QSPI1
    MEM_REGS_RAM_VALID  := TCMA TCMB SRAM
    MEM_REG_ROM_DEFAULT := EFLASH
    MEM_REG_RAM_DEFAULT := SRAM

else ifeq ($(CORE_NUM),CORE_1)# BR350P
    ABI                 ?= ilp32$(F_EXTENSION)
    ARCH                ?= rv32ima$(F_EXTENSION)cpn_zicsr_zifencei_zmmul_zba_zbb_zbc_zbkb_zbkc_zbkx_zbpbo_zbs_zknd_zkne_zknh_zksed_zksh_zpn_zpsfoperand
    ifeq ($(MCU_MODEL),BMCU_U)
        SPECS           := bmcu_u_core1.specs
    else
        SPECS           := be_u1000_core1.specs
    endif
    CDEFS               += -DCORE_BR350S1
    CDEFS               += -D_RV32_C1_
    ifeq ($(MCU_MODEL),BMCU_U)
        MATH_VER        := STANDARD# Use toolchain math lib strictly for core without in-ROM math funcs implementation
    endif
    STARTUP_SUFFIX      := c0_c1
    MEM_REGS_ROM_VALID  := TCMA TCMB EFLASH QSPI1
    MEM_REGS_RAM_VALID  := TCMA TCMB SRAM
    MEM_REG_ROM_DEFAULT := EFLASH
    MEM_REG_RAM_DEFAULT := SRAM

else ifeq ($(CORE_NUM),CORE_0_1)# BR350, unversal ISA for Core 0 and Core 1
    ABI                 ?= ilp32$(F_EXTENSION)
    ARCH                ?= rv32ima$(F_EXTENSION)cn_zicsr_zifencei_zba_zbb_zbc_zbkb_zbkc_zbkx_zbs_zknd_zkne_zknh_zksed_zksh
    ifeq ($(MCU_MODEL),BMCU_U)
        SPECS        := bmcu_u_core0.specs
    else
        SPECS        := be_u1000_core0.specs
    endif
    CDEFS               += -DCORE_BR350S1
    CDEFS               += -D_RV32_C0_C1
    CDEFS               += -D__BMCU_ROM1_DSP_H  # Blocks including _bmcu_rom1_dsp.h to core without this lib
    STARTUP_SUFFIX      := c0_c1
    MEM_REGS_ROM_VALID  := TCMA TCMB EFLASH QSPI1
    MEM_REGS_RAM_VALID  := TCMA TCMB SRAM
    MEM_REG_ROM_DEFAULT := EFLASH
    MEM_REG_RAM_DEFAULT := SRAM

else ifeq ($(CORE_NUM),CORE_2)# BM310
    ABI                 ?= ilp32
    ifeq ($(MCU_MODEL),BMCU_U)
        ARCH            ?= rv32ic_zicsr_zifencei_zba_zbb_zbc_zbs
        SPECS           := bmcu_u_core2.specs
    else
        ARCH            ?= rv32imc_zicsr_zifencei_zba_zbb_zbc_zbs
        SPECS           := be_u1000_core2.specs
    endif
    CDEFS               += -DCORE_BR310S14
    CDEFS               += -D_RV32_C2_
    CDEFS               += -D__BMCU_ROM1_DSP_H  # Blocks including _bmcu_rom1_dsp.h for core without in-ROM DSP funcs implementation
    MATH_VER            = STANDARD# Use toolchain math lib strictly for core without in-ROM math funcs implementation
    STARTUP_SUFFIX      := c2
    MEM_REGS_ROM_VALID  := TCMA TCMB
    MEM_REGS_RAM_VALID  := TCMA TCMB
    MEM_REG_ROM_DEFAULT := TCMA
    MEM_REG_RAM_DEFAULT := TCMB
endif# $(CORE_NUM)

# Set default RAM and ROM memory regions if corresponding parameters have not been specified explicitly
MEM_REG_ROM ?= $(MEM_REG_ROM_DEFAULT)## Memory area for the permanent part of the firmware
MEM_REG_RAM ?= $(MEM_REG_RAM_DEFAULT)## Memory area for the variable part of the firmware


ifeq ($(MATH_VER),STANDARD)
    CDEFS               += -D__BMCU_ROM0_MATH_H # Blocks including _bmcu_rom1_math.h to use standard math.h lib
else ifeq ($(MATH_VER),IN_ROM)
    ifeq ($(FLOAT_POINT_MODE),SOFT)
        $(error FLOAT_POINT_MODE value is $(FLOAT_POINT_MODE).  Set FLOAT_POINT_MODE to "HARD" to use ROM math library)
    endif# $(FLOAT_POINT_MODE)
    CDEFS               += -D_MATH_H            # Blocks including math.h from lib to use in-ROM lib version
    CFLAGS              += -include $(SDK_DIR)/Drivers/HAL/Inc/bmcu_rom0_math.h #including in-ROM math header
endif# $(MATH_VER)

#-... -- -.-. ..- .-.-.- ... -.. -.- .-.-.- ..--- ----- ..--- ..... -...- -.--.-
# Toolchain utilities setup
CC := $(CCACHE) $(TC_PREFIX)gcc
ifeq ($(AS),as)
	AS := $(TC_PREFIX)as
endif
LD := $(TC_PREFIX)gcc
OC := $(TC_PREFIX)objcopy
OD := $(TC_PREFIX)objdump
NM := $(TC_PREFIX)nm
SZ := $(TC_PREFIX)size
SR := $(TC_PREFIX)strip
RE := $(TC_PREFIX)readelf

# Omit all symbol information from the output file
# LDFLAGS += --strip-all
SZFLAGS += --format=berkeley --radix=10

#-... -- -.-. ..- .-.-.- ... -.. -.- .-.-.- ..--- ----- ..--- ..... -...- -.--.-
SRC_AUTOSEARCH ?= 1## Add source files from subdirectories
# Include .c, .s and .S files in all subdirectories of project and the build system direcotry
# Also add all directories, containing .h files, as header search directories
ifeq ($(SRC_AUTOSEARCH),1)
    SRC_DIR += $(realpath $(sort $(dir $(call rwildcard,$(PRJ_DIR)/,*.c))))

    SRC_DIR += $(realpath $(sort $(dir\
                            $(call rwildcard,$(PRJ_DIR)/,*.s)\
                            $(call rwildcard,$(PRJ_DIR)/,*.S))))

    INC_DIR += $(realpath $(sort $(dir $(call rwildcard,$(PRJ_DIR)/,*.h))))## Directories to search for header files

else ifeq ($(SRC_AUTOSEARCH),2)
    # Set the default list of source code directories
    SRC_DIR += $(PRJ_DIR)## List of directories with C source files

    # Set the default list of headers search directories
    INC_DIR += $(SRC_DIR)
endif# $(SRC_AUTOSEARCH) != {1,2} no implicite .c and .s/.S source directories will be used

# Add BSP directory to the source code dirs list
SRC_DIR += $(BSP_DIR)

# Exclude some source code directories
SRC_DIR_EXCLUDE ?=
SRC_DIR := $(filter-out $(SRC_DIR_EXCLUDE), $(SRC_DIR))

# Prepare list of source code files
CSRC += $(foreach dir,$(SRC_DIR),$(wildcard $(dir)/*.c))
ASRC ?= $(foreach dir,$(SRC_DIR),$(wildcard $(dir)/*.S))

# Add startup code depending on the MCU model and the core model
ASRC += $(SDK_DIR)/Tools/build/platform/$(MCU_MODEL)/startup_$(STARTUP_SUFFIX).S

# Exclude .c files
CSRC_EXCLUDE ?=
CSRC := $(filter-out $(CSRC_EXCLUDE), $(CSRC))## List of all C source files to be built
# Exclude assemblery files
ASRC_EXCLUDE ?=
ASRC := $(filter-out $(ASRC_EXCLUDE), $(ASRC))## List of all assembler source files to be built

# Check the existance of source files
MISSING_FILES += $(foreach file,$(CSRC) $(ASRC),$(shell test -f $(file) || echo -n $(file)))
$(foreach file,$(MISSING_FILES),$(info Missing file $(file)))

INC_DIR += $(SDK_DIR)/Drivers/BASIS/Core/Include
INC_DIR += $(SDK_DIR)/Drivers/BASIS/Device/BMCU/Include
INC_DIR += $(SDK_DIR)/Drivers/HAL/Inc
INC_DIR += $(BSP_DIR)

# Implicit header file inclusion
ifneq ($(BOARD),) # BOARD value has been already checked at this statement
    CFLAGS += -include $(BSP_DIR)/bsp.h
endif# $(BOARD)

#-... -- -.-. ..- .-.-.- ... -.. -.- .-.-.- ..--- ----- ..--- ..... -...- -.--.-
# Generate toolchain utilities build arguments
INCLUDES := $(addprefix -I,$(INC_DIR))
TARGET := $(OUT_DIR)/$(PRJ_NAME)
OBJS += $(addprefix $(OBJ_DIR)/,$(notdir $(CSRC:.c=.o) $(ASRC:.S=.o)))
DEPS += $(addprefix $(OBJ_DIR)/,$(notdir $(CSRC:.c=.d)))
VPATH := $(sort $(dir $(CSRC) $(ASRC)))

#-... -- -.-. ..- .-.-.- ... -.. -.- .-.-.- ..--- ----- ..--- ..... -...- -.--.-
ifeq ($(BUILD_TYPE),release)
    OPT   ?= s## Optimization level control: 0, 1, 2, fast, s, z, etc
    DEBUG  = 0## Debug level control: 0, gdb3, etc
    CDEFS += -DNDEBUG# Disable libc debug functions

else # Debug mode as default
    OPT   ?= g
    DEBUG  = gdb3
endif# $(BUILD_TYPE)

#-... -- -.-. ..- .-.-.- ... -.. -.- .-.-.- ..--- ----- ..--- ..... -...- -.--.-
ASFLAGS += -mabi=$(ABI)## Assembler related flags
ASFLAGS += -march=$(ARCH)

CFLAGS += -mabi=$(ABI)## C compiler related flags
CFLAGS += -march=$(ARCH)
CFLAGS += -specs=$(SPECS)
CFLAGS += -mcmodel=$(CMODEL)
CFLAGS += -std=c1x
CFLAGS += -O$(OPT)
CFLAGS += -g$(DEBUG)
CFLAGS += -save-temps
CFLAGS += -ffunction-sections
CFLAGS += -fdata-sections
CFLAGS += -fno-common
CFLAGS += -Wno-implicit
CFLAGS += -Wall
CFLAGS += -Wextra
# CFLAGS += -Wpedantic


# Compiler flags to generate dependency files
DEPFLAGS = -MD -MP -MF $(dir $@)$(basename $(@F)).d

#-... -- -.-. ..- .-.-.- ... -.. -.- .-.-.- ..--- ----- ..--- ..... -...- -.--.-
# Prepare linker script
include $(SDK_DIR)/Tools/build/platform/$(MCU_MODEL)/mem_map.mk

# Get the ldscript file in the following sequence:
# - if ldscript is explicitly specified - use it
# - else if there is some .ld file in project directory - use it
# - else if any of memory region is explicitly specified - generate from template
#   (if only one memory region was specified, the default value will be used as
#   the second one)
ifneq ($(GOAL_NAME_CLEAN), $(MAKECMDGOALS))
    ifndef LD_SCRIPT
        # If there is some .ld file in project directory or previously
        # generated script - use it as linker script
        LD_SCRIPT := $(firstword $(wildcard $(PRJ_DIR)/*.ld) $(wildcard $(OUT_DIR)/*.ld))

        ifneq ($(LD_SCRIPT),)
            # $(info The found .ld file be used as the linker script: $(notdir $(LD_SCRIPT)))

        else
            # Linker script not found. It will be generated automatically.
            # Validate the specified memory region.
            ifeq ($(findstring $(MEM_REG_ROM), $(MEM_REGS_ROM_VALID)),)
                $(error ROM memory region MEM_REG_ROM is unset or invalid: $(MEM_REG_ROM)$n \
                        Select one of next values: $(MEM_REGS_ROM_VALID)$n)
            endif# $(MEM_REG_ROM)

            ifeq ($(findstring $(MEM_REG_RAM), $(MEM_REGS_RAM_VALID)),)
                $(error RAM memory region MEM_REG_RAM is unset or invalid: $(MEM_REG_RAM)$n \
                        Select one of next values: $(MEM_REGS_RAM_VALID)$n)
            endif# $(MEM_REG_RAM)

            # ROM and RAM regions can be shifted by the specified offset
            # Could be used for multi-core or QSPI-based projects
            MEM_REG_ROM_OFFSET ?= 0x00## ROM region offset in hex format
            ifeq ($(MEM_REG_ROM), QSPI1)# Set default offset in case of ROM type is QSPI (keep space for the XIP loader)
                MEM_REG_ROM_OFFSET ?= $(MEM_REG_ROM_QSPI_DEFAULT_OFFSET)
            endif# $(MEM_REG_ROM)
            $(MEM_REG_ROM)_ORIGIN := $(shell printf "0x%X" $$(($($(MEM_REG_ROM)_ORIGIN) + $(MEM_REG_ROM_OFFSET))))
            # If there is user defined ROM or RAM size limit - use it. Can be useful for multi-core projects
            MEM_REG_ROM_SIZE ?=# Eliminate udefined variable make warning
            ifneq ($(MEM_REG_ROM_SIZE),)
                $(MEM_REG_ROM)_SIZE := $(MEM_REG_ROM_SIZE)
            endif# $(MEM_REG_ROM_SIZE)
            MEM_REG_ROM_ACCESS_RIGHTS := rwx
            ifneq ($(findstring $(MEM_REG_ROM), EFLASH QSPI1),)
                MEM_REG_ROM_ACCESS_RIGHTS := rx
            endif# $(MEM_REG_ROM)

            MEM_REG_RAM_OFFSET ?= 0x00## RAM region offset in hex format
            $(MEM_REG_RAM)_ORIGIN := $(shell printf "0x%X" $$(($($(MEM_REG_RAM)_ORIGIN) + $(MEM_REG_RAM_OFFSET))))
            MEM_REG_RAM_SIZE ?=# Eliminate udefined variable make warning
            ifneq ($(MEM_REG_RAM_SIZE),)
                $(MEM_REG_RAM)_SIZE := $(MEM_REG_RAM_SIZE)
            endif# $(MEM_REG_RAM_SIZE)
            MEM_REG_RAM_ACCESS_RIGHTS := rwx
            ifneq ($(findstring $(MEM_REG_RAM), SRAM),)
                MEM_REG_RAM_ACCESS_RIGHTS := rw
            endif# $(MEM_REG_RAM)

           # Locate .rodata section in the RAM memory region in order to increase
           # .rodata access speed
           DATA_START := .data
           RODATA_IN_RAM ?=# Eliminate udefined variable make warning
           RODATA_IN_RAM_MOVE ?=# Eliminate udefined variable make warning
           ifeq ($(RODATA_IN_RAM), 1)
               ifneq ($(MEM_REG_ROM),$(MEM_REG_RAM))# No need if ROM and RAM regions are the same
                   RODATA_IN_RAM_MOVE := > RAM AT
                   DATA_START := .rodata
               endif# $(MEM_REG_ROM),$(MEM_REG_RAM)
           endif# $(RODATA_IN_RAM)

            IMAGE_MAGIC_NUM ?= $($(MEM_REG_ROM)_MAGIC_NUMBER)## Special value used by the bootROM

            # Generate the linker script from template
            LD_SCRIPT := $(OUT_DIR)/generated.ld
            $(shell mkdir --parent $(OUT_DIR))
            $(shell sed \
            -e 's/#IMAGE_MAGIC_NUM/$(IMAGE_MAGIC_NUM)/g'\
            -e 's/#ROM_COMMENT/Region: $(MEM_REG_ROM); Offset: $(MEM_REG_ROM_OFFSET)/g' \
            -e 's/#ROM_ACCESS_RIGHTS/$(MEM_REG_ROM_ACCESS_RIGHTS)/g' \
            -e 's/#ROM_ORIGIN/$($(MEM_REG_ROM)_ORIGIN)/g' \
            -e 's/#ROM_SIZE/$($(MEM_REG_ROM)_SIZE)/g' \
            -e 's/#RAM_COMMENT/Region: $(MEM_REG_RAM); Offset: $(MEM_REG_RAM_OFFSET)/g' \
            -e 's/#RAM_ACCESS_RIGHTS/$(MEM_REG_RAM_ACCESS_RIGHTS)/g' \
            -e 's/#RAM_ORIGIN/$($(MEM_REG_RAM)_ORIGIN)/g' \
            -e 's/#RAM_SIZE/$($(MEM_REG_RAM)_SIZE)/g' \
            -e 's/#RODATA_IN_RAM_MOVE/$(RODATA_IN_RAM_MOVE)/g' \
            -e 's/#DATA_START/$(DATA_START)/g' \
            $(SDK_DIR)/Tools/build/ldscript_template.ld > $(LD_SCRIPT))

           # If ROM and RAM regions are the same - modify the generated linker script
           ifeq ($(MEM_REG_ROM),$(MEM_REG_RAM))
               $(shell sed -i \
               -e '/#DELLINE1/d' \
               -e 's/#ROM_NAME/RAM/g' \
               $(LD_SCRIPT))
           else
               $(shell sed -i \
               -e 's/#DELLINE1//g' \
               -e 's/#ROM_NAME/ROM/g' \
               $(LD_SCRIPT))
           endif# $(MEM_REG_ROM),$(MEM_REG_RAM)
        endif# $(LD_SCRIPT)
    endif# LD_SCRIPT
endif# $(GOAL_NAME_CLEAN)

#-... -- -.-. ..- .-.-.- ... -.. -.- .-.-.- ..--- ----- ..--- ..... -...- -.--.-
ifeq ($(LD), $(TC_PREFIX)ld)
    LIBDIR += $(TC_DIR)/newlib-nano/riscv32-none-elf/lib
    LDFLAGPREFIX =
else ifeq ($(LD), $(TC_PREFIX)gcc)
    LIBDIR =
    LDFLAGPREFIX = -Wl,
else
    $(error Invalid linker setup)
endif# $(LD)

LIBFLAGS := $(addprefix -L , $(LIBDIR))

PRINTF_FLOAT ?= 0## Enable/disable printf %f (floating point) format support
ifeq ($(PRINTF_FLOAT),1)
    LDLIBS += -u _printf_float
    $(info printf: floating point support enabled)
endif# $(PRINTF_FLOAT)

SCANF_FLOAT ?= 0## Enable/disable scanf %f (floating point) format support
ifeq ($(SCANF_FLOAT),1)
    LDLIBS += -u _scanf_float
    $(info scanf: floating point support enabled)
endif# $(SCANF_FLOAT)

LDFLAGS += -mabi=$(ABI)## Linker related flags
LDFLAGS += -march=$(ARCH)
LDFLAGS += -specs=$(SPECS)
LDFLAGS += -L $(PRJ_DIR)
LDFLAGS += -T $(LD_SCRIPT)
LDFLAGS += -nostdlib
LDFLAGS += -nostartfiles
LDFLAGS += $(LDFLAGPREFIX)-entry="_start"
LDFLAGS += $(LDFLAGPREFIX)-gc-sections
LDFLAGS += $(LDFLAGPREFIX)-no-warn-rwx-segments
LDFLAGS += $(LDFLAGPREFIX)-cref
LDFLAGS += $(LDFLAGPREFIX)-print-memory-usage
LDFLAGS += $(LDFLAGPREFIX)-Map=$(TARGET).map
LDFLAGS += $(LDFLAGPREFIX)-build-id
LDFLAGS += $(LIBFLAGS)
LDFLAGS += -static

# C definitions could be used in source code
CDEFS += -D'PRJ_NAME="$(PRJ_NAME)"'## C definitions
CDEFS += -D'BUILD_TYPE="$(BUILD_TYPE)"'
ifneq ($(BOARD),)
    CDEFS += -D'$(BOARD)'# Required for some examples
    CDEFS += -D'BOARD="$(BOARD)"'
endif
CDEFS += -D'$(MCU_MODEL)'# Required for the BASIS library (see bmcu_common.h)
CDEFS += -D'MCU_MODEL="$(MCU_MODEL)"'
CDEFS += -D'$(CORE_NUM)'
CDEFS += -D'CORE_NUM="$(CORE_NUM)"'

RAM_ACCESS_OFFSET ?= 0x00# RAM adress offset depending on Front Port is used
ifneq ($(MEM_REG_RAM),SRAM)
    RAM_ACCESS_OFFSET := $(FRONT_PORT_OFFSET)
endif
CDEFS += -D'RAM_ACCESS_OFFSET=$(RAM_ACCESS_OFFSET)'

#-... -- -.-. ..- .-.-.- ... -.. -.- .-.-.- ..--- ----- ..--- ..... -...- -.--.-
.DEFAULT_GOAL: all
# Keep all .o intermediate files
.PRECIOUS: $(OBJ_DIR)/%.o
# Keep all intermediate files
.SECONDARY:
.DELETE_ON_ERROR:
.ONESHELL:
# Disable parallel execution in case of make goal "clean" (example: "make clean all")
ifeq ($(findstring $(GOAL_NAME_CLEAN), $(MAKECMDGOALS)), $(GOAL_NAME_CLEAN))
    .NOTPARALLEL:
else ifeq ($(NOTPARALLEL_MAKE_REQ_OK),true)
    .NOTPARALLEL: clear# Add here rules, that should not be built in parallel
endif#  $(GOAL_NAME_CLEAN)

# Set of the main targets. See also service.mk file for some service targets
.PHONY: all

#-... -- -.-. ..- .-.-.- ... -.. -.- .-.-.- ..--- ----- ..--- ..... -...- -.--.-
ifeq ($(BUILD_TYPE),release)
    CFLAGS  += -flto
    LDFLAGS += -flto
    LDFLAGS += $(LDFLAGPREFIX)-strip-all

    # Default target. Build firmware in all supported formats
    all: elf bin hex## Default target
else
    CFLAGS  += -fverbose-asm
    CFLAGS  += -fstack-usage

    # Default target. Build firmware in all supported formats
    all: elf bin hex disasm lss sym# See "all" tarteg for BUILD_TYPE=release
endif# $(BUILD_TYPE)

#-... -- -.-. ..- .-.-.- ... -.. -.- .-.-.- ..--- ----- ..--- ..... -...- -.--.-
# Incude makefile with self debug variable and procedure
include $(SDK_DIR)/Tools/build/mk_debug.mk

elf:    $(TARGET).elf## Build firmware in debuggable format (.elf)
hex:    $(TARGET).hex## Build firmware in intel HEX format (.hex)
bin:    $(TARGET).bin## Build firmware in binary format, suitable for memroy flashing (.bin)
disasm: $(TARGET).disasm## Prepare disassembly of the built firmware for more convenient debug (.disasm)
lss:    $(TARGET).lss## Generate firmware assembler listing (.lss)
sym:    $(TARGET).sym## Generate firmware symbols list (.sym)

CFLAGS_OVERRIDE ?= ## Override C-compiler options with user-specified ones
ifneq ($(CFLAGS_OVERRIDE),)
    CFLAGS := $(CFLAGS_OVERRIDE)
endif

CDEFS_OVERRIDE ?= ## Override definitions with user-specified ones
ifneq ($(CDEFS_OVERRIDE),)
    CDEFS := $(CDEFS_OVERRIDE)
endif# $(CFLAGS_OVERRIDE)

# Create object files from *.c sources
$(OBJ_DIR)/%.o: %.c $(MAKEFILE_LIST)
	$(Q)mkdir -p $(dir $@)
	printf "[CC ] %-20s -> %s\n" $(notdir $<) $(notdir $@)
	$(Q)$(CC) $(CFLAGS) $(CDEFS) $(DEPFLAGS) $(INCLUDES) -c $< -o $@

# Create object files from *.S sources
$(OBJ_DIR)/%.o: %.S $(MAKEFILE_LIST)
	$(Q)mkdir -p $(dir $@)
	printf "[AS ] %-20s -> %s\n" $(notdir $<) $(notdir $@)
	$(Q)$(AS) $(ASFLAGS) $(INCLUDES) -c $< -o $@

# Create ELF output file from object files
%.elf: $(OBJS)
	$(Q)mkdir -p $(dir $@)
	printf "[LD ] %-20s  : %s\n" $(notdir $(LD_SCRIPT)) $(notdir $@)
	$(Q)$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)
	$(Q)echo "Size of $@:"
	$(Q)$(SZ) $(SZFLAGS) $@
	$(RE) -n $@ | grep 'Build ID'

# Create BIN output file from ELF output file
%.bin: %.elf
	printf "[BIN] %-20s -> %s\n" $(notdir $<) $(notdir $@)
	$(Q)$(OC) -O binary $< $@

# Create HEX output file from ELF output file
%.hex: %.elf
	printf "[HEX] %-20s -> %s\n" $(notdir $<) $(notdir $@)
	$(Q)$(OC) -O ihex $< $@

# Create extended listing from ELF output file
%.lss: %.elf
	printf "[LSS] %-20s -> %s\n" $(notdir $<) $(notdir $@)
	$(Q)$(OD) -h -t -S -C $< > $@

# Create disassemble listing from ELF output file
%.disasm: %.elf
	printf "[DIS] %-20s -> %s\n" $(notdir $<) $(notdir $@)
	$(Q)$(OD) -fhtDS $< > $@

# Create a symbol table from ELF output file
%.sym: %.elf
	printf "[SYM] %-20s -> %s\n" $(notdir $<) $(notdir $@)
	$(Q)$(NM) -n $< > $@

# Print make variable
print-%:
	@echo $($*)

# Be careful with "rm -rf" command
.PHONY: $(GOAL_NAME_CLEAN)
$(GOAL_NAME_CLEAN):## Delete build artifacts (tipicaly "clean")
	$(Q)rm -rf $(BUILD_DIR)

-include $(DEPS)
#-... -- -.-. ..- .-.-.- ... -.. -.- .-.-.- ..--- ----- ..--- ..... -...- -.--.-
