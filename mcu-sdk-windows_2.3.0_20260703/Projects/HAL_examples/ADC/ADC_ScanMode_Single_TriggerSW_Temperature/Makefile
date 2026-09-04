# See Tools/build/Makefile_template for more information
SDK_DIR = $(realpath $(CURDIR)/../../../..)

BOARD = EVU_BA_2_5

PRINTF_FLOAT := 1# Print the temperature in floating poin format

CSRC += $(SDK_DIR)/Drivers/HAL/Src/bmcu_cru.c
CSRC += $(SDK_DIR)/Drivers/HAL/Src/bmcu_adc.c
CSRC += $(SDK_DIR)/Drivers/HAL/Src/bmcu_eflash.c

# Include the main makefile
include $(SDK_DIR)/Tools/build/common.mk
