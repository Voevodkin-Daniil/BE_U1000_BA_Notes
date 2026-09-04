CORE_NUM = CORE_0

include ../config.mk

ifdef CODE_SHIFT
	MEM_REG_ROM_OFFSET = 0x00
	MEM_REG_ROM_SIZE = $(CODE_SHIFT)

	MEM_REG_RAM_OFFSET = 0x00
	MEM_REG_RAM_SIZE = 0x4000# 16K
else
	CODE_SHIFT = 10000
endif

ifeq ($(MEM_REG_ROM), QSPI1)
	MEM_REG_ROM_OFFSET := $(MEM_REG_ROM_OFFSET) + $(QSPI_APP_OFFSET)
endif

CDEFS += "-DCODE_SHIFT = $(CODE_SHIFT)"

# Include the main makefile
include $(SDK_DIR)/Tools/build/common.mk