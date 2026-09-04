# See Tools/build/Makefile_template for more information
SDK_DIR = $(realpath $(CURDIR)/../../../..)

BOARD = EVU_BA_2_5# Comment/delete this line if a custom board used
BOARD_SUPPORTED = EVU_BA_1_2 EVU_BA_2_0 EVU_BA_2_1 EVU_BA_2_3 EVU_BA_2_5# Boards with user button

ifneq ($(BOARD),)
    ifeq ($(findstring $(BOARD), $(BOARD_SUPPORTED)),)
        $(error This example intended to be run only on next boards: $(BOARD_SUPPORTED))
    endif
endif

# Explicitly exclude BSP
SRC_DIR_EXCLUDE += $(SDK_DIR)/BSP/$(BOARD)

CSRC += $(SDK_DIR)/Drivers/HAL/Src/bmcu_cru.c
CSRC += $(SDK_DIR)/Drivers/HAL/Src/bmcu_gpio.c

# Include the main makefile
include $(SDK_DIR)/Tools/build/common.mk
