# See Tools/build/Makefile_template for more information
SDK_DIR = $(realpath $(CURDIR)/../../../..)

BOARD = EVU_BA_2_5# Comment/delete this line if a custom board used
BOARD_SUPPORTED = EVU_BA_1_2 EVU_BA_2_0 EVU_BA_2_1 EVU_BA_2_3 EVU_BA_2_5# Boards with user button

ifneq ($(BOARD),)
    ifeq ($(findstring $(BOARD), $(BOARD_SUPPORTED)),)
        $(error This example intended to be run only on next boards: $(BOARD_SUPPORTED))
    endif
endif

# Configure Input interrupt type: edge or level
# Set BTN_IT_TYPE = EDGE for edge type interrupt (default)
# Set BTN_IT_TYPE = LEVEL for level type interrupt
BTN_IT_TYPE_VALID = EDGE LEVEL
ifdef BTN_IT_TYPE
    ifeq ($(findstring $(BTN_IT_TYPE),$(BTN_IT_TYPE_VALID)),)
        $(error Invalid BTN_IT_TYPE parameter. Select one of the following: $(BTN_IT_TYPE_VALID))
    endif
endif
BTN_IT_TYPE ?= EDGE
CDEFS += -D'BTN_IT_TYPE = GPIO_IT_TYPE_$(BTN_IT_TYPE)'

# Configure Input interrupt mode: one edge or both edges
# Set BTN_IT_MODE = ONE for only one edge mode interrupt (default)
# Set BTN_IT_MODE = BOTH for both edges mode interrupt
BTN_IT_MODE_VALID = ONE BOTH
ifdef BTN_IT_MODE
    ifeq ($(findstring $(BTN_IT_MODE),$(BTN_IT_MODE_VALID)),)
        $(error Invalid BTN_IT_MODE parameter. Select one of the following: $(BTN_IT_MODE_VALID))
    endif
endif
BTN_IT_MODE ?= ONE
ifeq ($(BTN_IT_MODE),ONE)
    CDEFS += -D'BTN_IT_MODE = GPIO_IT_BOTHEDGE_DISABLE'
else
    CDEFS += -D'BTN_IT_MODE = GPIO_IT_BOTHEDGE_ENABLE'
endif

# Configure Input interrupt polarity: low or high
# Set BTN_IT_POL = LOW for low polarity interrupt (default)
# Set BTN_IT_POL = HIGH for high polarity interrupt
BTN_IT_POL_VALID = LOW HIGH
ifdef BTN_IT_POL
    ifeq ($(findstring $(BTN_IT_POL),$(BTN_IT_POL_VALID)),)
        $(error Invalid BTN_IT_POL parameter. Select one of the following: $(BTN_IT_POL_VALID))
    endif
endif
BTN_IT_POL ?= LOW
CDEFS += -D'BTN_IT_POL = GPIO_IT_POL_$(BTN_IT_POL)'

# Explicitly exclude BSP
SRC_DIR_EXCLUDE += $(SDK_DIR)/BSP/$(BOARD)

CSRC += $(SDK_DIR)/Drivers/HAL/Src/bmcu_cru.c
CSRC += $(SDK_DIR)/Drivers/HAL/Src/bmcu_gpio.c

# Include the main makefile
include $(SDK_DIR)/Tools/build/common.mk
