CORE_NUM = CORE_1

include ../config.mk

ifeq ($(MEM_REG_ROM), QSPI1)
	CODE_SHIFT := $(CODE_SHIFT) + $(QSPI_APP_OFFSET)
endif

MEM_REG_ROM_OFFSET = $(CODE_SHIFT)

MEM_REG_ROM_SIZE = $($(MEM_REG_ROM)_SIZE) - ($(CODE_SHIFT))

MEM_REG_RAM_OFFSET = 0x4000
MEM_REG_RAM_SIZE = 0x4000# 16K

# Include the main makefile
include $(SDK_DIR)/Tools/build/common.mk