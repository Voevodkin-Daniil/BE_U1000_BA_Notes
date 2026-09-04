# See Tools/build/Makefile_template for more information
SDK_DIR = $(realpath $(CURDIR)/../../../../..)

BOARD = EVU_BA_2_5
CORE_NUM = CORE_1

CDEFS   += -D__BMCU_ROM0_MATH_H
INC_DIR += $(realpath $(CURDIR)/../Common/source/include)
LDLIBS  += $(realpath $(CURDIR)/../Common/lib/dsp_lib_p-ext.a)

MEM_REG_ROM_OFFSET = 0x20000# 128K shift
MEM_REG_ROM_SIZE = 0x20000# 128K

MEM_REG_RAM_OFFSET = 0x4000# 16K shift
MEM_REG_RAM_SIZE = 0x4000# 16K

# Include the main makefile
include $(SDK_DIR)/Tools/build/common.mk
