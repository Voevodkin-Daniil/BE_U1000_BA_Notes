# See Tools/build/Makefile_template for more information
SDK_DIR = $(realpath $(CURDIR)/../../../../..)
PROJ_DIR = $(realpath $(CURDIR)/..)

CSRC += $(SDK_DIR)/Drivers/HAL/Src/bmcu_tim.c
CSRC += $(SDK_DIR)/Drivers/HAL/Src/bmcu_qspi.c

#CFLAGS += -Wincompatible-pointer-types

BOARD = EVU_BA_2_0
MCU_MODEL = BMCU_U1000
CORE_NUM = CORE_0

MEM_REG_ROM_OFFSET = 0x00
MEM_REG_ROM_SIZE = 0x20000# 128K

MEM_REG_RAM_OFFSET = 0x00
MEM_REG_RAM_SIZE = 0x4000# 16K

INC_DIR +=$(PROJ_DIR)

CSRC += $(SDK_DIR)/Drivers/HAL/Src/bmcu_cru.c
CSRC += $(SDK_DIR)/Drivers/HAL/Src/bmcu_uart.c
CSRC += $(SDK_DIR)/Drivers/HAL/Src/bmcu_eflash.c
CSRC += $(PROJ_DIR)/cli.c
CSRC += $(PROJ_DIR)/printf_off.c
CSRC += $(PROJ_DIR)/qspi.c
CSRC += $(PROJ_DIR)/qspi_flash.c
CSRC += $(PROJ_DIR)/syscalls.c

CDEFS += -DMT25Q
CDEFS += -DIF_INSTANCE=QSPI1

CFLAGS += -funroll-all-loops
OPT ?= fast


#CDEFS += -DTURN_OFF_PRINTF
CDEFS += -DPRINT_CLKS


# Include the main makefile
include $(SDK_DIR)/Tools/build/common.mk
