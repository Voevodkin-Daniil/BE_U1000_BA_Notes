# See Tools/build/Makefile_template for more information
SDK_DIR = $(realpath $(CURDIR)/../../../../..)

BOARD = EVU_BA_2_5

CDEFS += -DCFG_TUSB_MCU=OPT_MCU_BMCU
CDEFS += -DBOARD_TUD_MAX_SPEED=OPT_MODE_HIGH_SPEED

CSRC += $(SDK_DIR)/Middlewares/Third_Party/TinyUSB/src/tusb.c
CSRC += $(SDK_DIR)/Middlewares/Third_Party/TinyUSB/src/common/tusb_fifo.c
CSRC += $(SDK_DIR)/Middlewares/Third_Party/TinyUSB/src/device/usbd_control.c
CSRC += $(SDK_DIR)/Middlewares/Third_Party/TinyUSB/src/device/usbd.c
CSRC += $(SDK_DIR)/Middlewares/Third_Party/TinyUSB/src/class/msc/msc_device.c
CSRC += $(SDK_DIR)/Middlewares/Third_Party/TinyUSB/src/portable/bmcu/dcd_bmcu.c
CSRC += $(SDK_DIR)/Middlewares/Third_Party/TinyUSB/hw/bsp/bmcu/family.c
CSRC += $(SDK_DIR)/Drivers/HAL/Src/bmcu_cru.c
CSRC += $(SDK_DIR)/Drivers/HAL/Src/bmcu_dma.c
CSRC += $(SDK_DIR)/Drivers/HAL/Src/bmcu_eflash.c
CSRC += $(SDK_DIR)/Drivers/HAL/Src/bmcu_qspi.c

INC_DIR += $(SDK_DIR)/Middlewares/Third_Party/TinyUSB/src
INC_DIR += $(SDK_DIR)/Middlewares/Third_Party/TinyUSB/hw

# Include the main makefile
include $(SDK_DIR)/Tools/build/common.mk
