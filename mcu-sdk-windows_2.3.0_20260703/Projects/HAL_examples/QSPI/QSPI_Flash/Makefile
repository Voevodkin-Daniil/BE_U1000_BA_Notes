# See Tools/build/Makefile_template for more information
SDK_DIR = $(realpath $(CURDIR)/../../../..)

BOARD = EVU_BA_2_5

CDEFS += -DIF_INSTANCE=QSPI1
CDEFS += -DMT25Q# SPI flash memory type

CSRC += $(SDK_DIR)/Drivers/HAL/Src/bmcu_cru.c
CSRC += $(SDK_DIR)/Drivers/HAL/Src/bmcu_gpio.c
CSRC += $(SDK_DIR)/Drivers/HAL/Src/bmcu_uart.c
CSRC += $(SDK_DIR)/Drivers/HAL/Src/bmcu_qspi.c
CSRC += $(SDK_DIR)/Drivers/HAL/Src/bmcu_eflash.c

# Include the main makefile
include $(SDK_DIR)/Tools/build/common.mk
