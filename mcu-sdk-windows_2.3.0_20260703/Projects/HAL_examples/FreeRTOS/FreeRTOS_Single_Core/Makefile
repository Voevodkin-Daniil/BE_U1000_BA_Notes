# See Tools/build/Makefile_template for more information
SDK_DIR = $(realpath $(CURDIR)/../../../..)

AS = $(TC_PREFIX)gcc
ASFLAGS += -mcmodel=$(CMODEL)
ASFLAGS += -O$(OPT)
ASFLAGS += -g$(DEBUG)
ASFLAGS += -specs=$(SPECS)
ASFLAGS += -x assembler-with-cpp

RTOS_SOURCE_DIR = $(realpath $(SDK_DIR)/Middlewares/Third_Party/FreeRTOS/FreeRTOS/Source)

BOARD = EVU_BA_2_5

MEM_REG_ROM ?= EFLASH
MEM_REG_ROM_SUPPORTED = TCMA TCMB EFLASH

ifneq ($(MEM_REG_ROM),)
    ifeq ($(findstring $(MEM_REG_ROM), $(MEM_REG_ROM_SUPPORTED)),)
        $(error This example intended to be run only on next ROM types: $(MEM_REG_ROM_SUPPORTED))
    endif
endif

INC_DIR += -I $(RTOS_SOURCE_DIR)/include
INC_DIR += -I $(RTOS_SOURCE_DIR)/portable/ThirdParty/GCC/RISC-V/Baikal
INC_DIR += -I $(RTOS_SOURCE_DIR)/portable/ThirdParty/GCC/RISC-V/Baikal/chip_specific_extensions/RISCV_no_extensions

CSRC += $(RTOS_SOURCE_DIR)/list.c
CSRC += $(RTOS_SOURCE_DIR)/queue.c
CSRC += $(RTOS_SOURCE_DIR)/tasks.c
CSRC += $(RTOS_SOURCE_DIR)/portable/MemMang/heap_4.c
CSRC += $(RTOS_SOURCE_DIR)/portable/ThirdParty/GCC/RISC-V/Baikal/port.c

ASRC += $(RTOS_SOURCE_DIR)/portable/ThirdParty/GCC/RISC-V/Baikal/portASM.S

# Include the main makefile
include $(SDK_DIR)/Tools/build/common.mk
