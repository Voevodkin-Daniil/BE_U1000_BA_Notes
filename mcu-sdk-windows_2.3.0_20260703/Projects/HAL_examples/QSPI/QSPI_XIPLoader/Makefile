# See Tools/build/Makefile_template for more information
SDK_DIR = $(realpath $(CURDIR)/../../../..)

BOARD ?= EVU_BA_2_5
MEM_REG_ROM ?= TCMA# Tipically XIP loader has to be built as TCMA project

CDEFS += -DMT25Q# SPI flash memory type

# Set this variable accordint to the QSPI application address
# XIP loader build binary size is ~14 kB
QSPI_APP_OFFSET ?= 0x10000UL# 64 kB
CDEFS += -D'__QSPI_APP_ADDR__=$(QSPI_APP_OFFSET)'

# Set CCLK clock (MHz)
CLK_CCLK ?= 200
CDEFS += -DCLK_CCLK=$(CLK_CCLK)

# Set QSPI clock (MHz)
QSPI_CLK ?= 25
CDEFS += -DQSPI_CLK=$(QSPI_CLK)

CSRC += $(SDK_DIR)/Drivers/HAL/Src/bmcu_cru.c
CSRC += $(SDK_DIR)/Drivers/HAL/Src/bmcu_eflash.c
CSRC += $(SDK_DIR)/Drivers/HAL/Src/bmcu_gpio.c
CSRC += $(SDK_DIR)/Drivers/HAL/Src/bmcu_qspi.c

ifeq ($(BUILD_TYPE),release)
    CSRC_EXCLUDE += $(CURDIR)/syscalls.c
endif

# Include the main makefile
include $(SDK_DIR)/Tools/build/common.mk
