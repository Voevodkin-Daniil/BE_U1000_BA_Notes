# See Tools/build/Makefile_template for more information
SDK_DIR = $(realpath $(CURDIR)/../../../..)

BOARD = EVU_BA_2_1

CSRC += $(SDK_DIR)/Drivers/HAL/Src/bmcu_cru.c
CSRC += $(SDK_DIR)/Drivers/HAL/Src/bmcu_gpio.c
CSRC += $(SDK_DIR)/Drivers/HAL/Src/bmcu_pwma.c

# Include the main makefile
include $(SDK_DIR)/Tools/build/common.mk
