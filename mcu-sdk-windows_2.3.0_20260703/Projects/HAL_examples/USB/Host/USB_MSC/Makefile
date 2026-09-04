# See Tools/build/Makefile_template for more information
SDK_DIR = $(realpath $(CURDIR)/../../../../..)

BOARD = EVU_BA_2_5

CDEFS += -DCFG_TUSB_MCU=OPT_MCU_BMCU
CDEFS += -DBOARD_TUH_MAX_SPEED=OPT_MODE_HIGH_SPEED

CSRC += $(SDK_DIR)/Middlewares/Third_Party/TinyUSB/src/tusb.c
CSRC += $(SDK_DIR)/Middlewares/Third_Party/TinyUSB/src/common/tusb_fifo.c
CSRC += $(SDK_DIR)/Middlewares/Third_Party/TinyUSB/src/host/usbh.c
CSRC += $(SDK_DIR)/Middlewares/Third_Party/TinyUSB/src/host/hub.c
CSRC += $(SDK_DIR)/Middlewares/Third_Party/TinyUSB/src/class/msc/msc_host.c
CSRC += $(SDK_DIR)/Middlewares/Third_Party/TinyUSB/src/portable/bmcu/hcd_bmcu.c
CSRC += $(SDK_DIR)/Middlewares/Third_Party/TinyUSB/hw/bsp/bmcu/family.c
CSRC += $(SDK_DIR)/Middlewares/Third_Party/FatFs/source/ff.c
CSRC += $(SDK_DIR)/Drivers/HAL/Src/bmcu_cru.c
CSRC += $(SDK_DIR)/Drivers/HAL/Src/bmcu_eflash.c
CSRC += $(SDK_DIR)/Drivers/HAL/Src/bmcu_gpio.c
CSRC += $(SDK_DIR)/Drivers/HAL/Src/bmcu_uart.c

INC_DIR += $(SDK_DIR)/Middlewares/Third_Party/TinyUSB/src
INC_DIR += $(SDK_DIR)/Middlewares/Third_Party/TinyUSB/hw
INC_DIR += $(SDK_DIR)/Middlewares/Third_Party/FatFs/source

# Include the main makefile
include $(SDK_DIR)/Tools/build/common.mk
