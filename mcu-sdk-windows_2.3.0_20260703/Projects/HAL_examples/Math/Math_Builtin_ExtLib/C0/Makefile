# See Tools/build/Makefile_template for more information
SDK_DIR = $(realpath $(CURDIR)/../../../../..)

CSRC += $(SDK_DIR)/Drivers/HAL/Src/bmcu_cru.c
CSRC += $(SDK_DIR)/Drivers/HAL/Src/bmcu_eflash.c

BOARD = EVU_BA_2_5
CORE_NUM = CORE_0

MEM_REG_ROM_SIZE = 0x20000# 128K

MEM_REG_RAM_SIZE = 0x4000# 16K

CDEFS   += -D_MATH_H_
INC_DIR += $(realpath $(CURDIR)/../Common/source/include)
LDLIBS  += $(realpath $(CURDIR)/../Common/lib/dsp_lib.a)

# Include the main makefile
include $(SDK_DIR)/Tools/build/common.mk
