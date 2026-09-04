# See Tools/build/Makefile_template for more information
SDK_DIR = $(realpath $(CURDIR)/../../../..)

CDEFS += -D'PERIOD=1000' #1Main loop period (ms)
#CDEFS += -D'USE_IRQ' # Uncomment to enable DMA IRQ and ISRs

# Use only SRAM memory as the linker RAM region
MEM_REG_RAM := SRAM
ifneq ($(MEM_REG_RAM),SRAM)
    $(error This example intended to use SRAM memory as RAM region. Current value: $(MEM_REG_RAM))
endif

# Build only for Core 0 or Core 1
CORE_NUM_SUPPORTED := CORE_0 CORE_1 CORE_0_1
ifneq ($(CORE_NUM),)
    ifeq ($(findstring $(CORE_NUM), $(CORE_NUM_SUPPORTED)),)
        $(error This example intended to be built for next CORE_NUM values: $(CORE_NUM_SUPPORTED). Current value: $(CORE_NUM))
    endif
endif

CSRC += $(SDK_DIR)/Drivers/HAL/Src/bmcu_cru.c
CSRC += $(SDK_DIR)/Drivers/HAL/Src/bmcu_spi.c
CSRC += $(SDK_DIR)/Drivers/HAL/Src/bmcu_dma.c
CSRC += $(SDK_DIR)/Drivers/HAL/Src/bmcu_eflash.c

# Explicitly exclude BSP
SRC_DIR_EXCLUDE += $(SDK_DIR)/BSP/$(BOARD)

ifeq ($(BUILD_TYPE),release)
    CSRC_EXCLUDE += $(CURDIR)/syscalls.c
endif

# Include the main makefile
include $(SDK_DIR)/Tools/build/common.mk
