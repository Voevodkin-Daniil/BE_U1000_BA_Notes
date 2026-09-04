# See Tools/build/Makefile_template for more information
SDK_DIR = $(realpath $(CURDIR)/../../../..)

BOARD = EVU_BA_2_5

CSRC += $(SDK_DIR)/Drivers/HAL/Src/bmcu_cru.c
CSRC += $(SDK_DIR)/Drivers/HAL/Src/bmcu_i2s.c
CSRC += $(SDK_DIR)/Drivers/HAL/Src/bmcu_spi.c
CSRC += $(SDK_DIR)/Drivers/HAL/Src/bmcu_dma.c

# Include the main makefile
include $(SDK_DIR)/Tools/build/common.mk
