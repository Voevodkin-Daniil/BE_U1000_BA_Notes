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
include $(SDK_DIR)/BSP/$(BOARD)/bsp.mk

INC_DIR += -I $(RTOS_SOURCE_DIR)/include
INC_DIR += -I $(RTOS_SOURCE_DIR)/portable/ThirdParty/GCC/RISC-V/BaikalSMP
INC_DIR += -I $(RTOS_SOURCE_DIR)/portable/ThirdParty/GCC/RISC-V/BaikalSMP/chip_specific_extensions/RISCV_no_extensions

CSRC += $(RTOS_SOURCE_DIR)/list.c
CSRC += $(RTOS_SOURCE_DIR)/queue.c
CSRC += $(RTOS_SOURCE_DIR)/tasks.c

CSRC += $(RTOS_SOURCE_DIR)/portable/MemMang/heap_4.c
CSRC += $(RTOS_SOURCE_DIR)/portable/ThirdParty/GCC/RISC-V/BaikalSMP/port.c
CSRC += $(RTOS_SOURCE_DIR)/portable/ThirdParty/GCC/RISC-V/BaikalSMP/sync.c

ASRC += startup.S
ASRC += startup1.S
ASRC += $(RTOS_SOURCE_DIR)/portable/ThirdParty/GCC/RISC-V/BaikalSMP/portASM.S
ASRC_EXCLUDE += $(SDK_DIR)/Tools/build/platform/$(MCU_MODEL)/startup_c0_c1.S

# Include the main makefile
include $(SDK_DIR)/Tools/build/common.mk
