SHELL=/bin/bash

# Select core run mode (RUN_CORE0, RUN_CORE01_PAR)
RUN_MODE ?= RUN_CORE0

CLK_CCLK ?= 200

# SDK
SDK_DIR = $(realpath $(CURDIR)/../../../..)

# Toolchain
TC_PREFIX = $(SDK_DIR)/Tools/toolchain/riscv32-none-elf/bin/riscv32-none-elf
OBJCOPY = $(TC_PREFIX)-objcopy

BUILD_TYPE ?= debug

ifneq ($(RUN_MODE),$(filter $(RUN_MODE), RUN_CORE0 RUN_CORE01_PAR RUN_CORE1))
	$(error RUN_MODE $(RUN_MODE) is not supported)
endif

# Coremark parameter: Iterations number
PARAM_ITERATIONS ?= 10000

# Optimization level (0, 1, 2, 3, s, fast)
OPT ?= fast

ifneq ($(OPT),$(filter $(OPT), 0 1 2 3 s fast))
	$(error OPT $(OPT) is not supported)
endif

# Debugging format
DEBUG ?= 3

# Target memory (TCMA, EFLASH, QSPI1)
MEM_REG_ROM ?= EFLASH

# For MEM_REG_ROM = QSPI set freqency, (25, 50)
QSPI_CLK ?= 25

ifneq ($(MEM_REG_ROM),$(filter $(MEM_REG_ROM),TCMA EFLASH QSPI1))
	$(error MEM_REG_ROM $(MEM_REG_ROM) is not supported)

endif

# Target core clock (MHz)(25, 100, 200)
CLK_CCLK ?= 200

# System parameter
ifneq ($(CLK_CCLK),$(filter $(CLK_CCLK),25 100 200))
	$(error CLK_CCLK $(CLK_CCLK) is not supported)
endif

# Output binary image filename
ifneq ($(MEM_REG_ROM), QSPI1)
	IMAGE_NAME ?= coremark_$(PARAM_ITERATIONS)-$(RUN_MODE)-CCLK_$(CLK_CCLK)-MEM_$(MEM_REG_ROM)-OPT_$(OPT).bin
else
	QSPI_APP_OFFSET = 0x10000
	IMAGE_NAME ?= coremark_$(PARAM_ITERATIONS)-$(RUN_MODE)-CCLK_$(CLK_CCLK)-MEM_$(MEM_REG_ROM)_$(QSPI_CLK)-OPT_$(OPT).bin
	export  QSPI_APP_OFFSET
endif

TRANSIT_PARAMS = OPT SDK_DIR BUILD_TYPE MEM_REG_ROM CLK_CCLK PARAM_ITERATIONS

# The path to QSPI_XIPLoader project
XIP_LOADER_DIR ?= $(SDK_DIR)/Projects/HAL_examples/QSPI/QSPI_XIPLoader

BORDER_ALIGN = 64

export $(TRANSIT_PARAMS)
# INCLUDES_TO_PASS="$(INCLUDES)"

.DEFAULT_GOAL: all
.ONESHELL:
.PHONY : all clean dependents

# Targets
ifeq ($(RUN_MODE),$(filter $(RUN_MODE), RUN_CORE01_PAR RUN_CORE1))
all: prebuild countsize dependents cat
else 
all: dependents cat
endif


clean:
	cd C0 && $(MAKE) clean
	cd ..
	cd C1 && $(MAKE) clean
	cd ..
	rm -rf output
	sed '/^CODE_SHIFT /d' -i config.mk

prebuild:
	cd C0 && $(MAKE) clean && $(MAKE) all --no-silent
	cd ..

countsize:
	@$(eval CODE_SIZE = $(shell wc -c C0/output/$(BUILD_TYPE)/C0.bin | cut -d ' ' -f1))
	@$(eval TMP = $(shell expr $(CODE_SIZE) / $(BORDER_ALIGN) + 1 ))
	@$(eval CODE_SHIFT = $(shell echo $$(( $(TMP) * $(BORDER_ALIGN) ))))
	@echo "CODE_SHIFT = $(CODE_SHIFT)" >> config.mk

dependents:
	$(info $(MAKE))
	cd C0 && $(MAKE) clean && $(MAKE) all 
	cd ..
ifeq ($(RUN_MODE),$(filter $(RUN_MODE), RUN_CORE01_PAR RUN_CORE1))
	cd C1 && $(MAKE) clean && $(MAKE) all
	cd ..
endif

cat:
    # Create output directory
	mkdir -p output

ifeq ($(RUN_MODE),$(filter $(RUN_MODE), RUN_CORE01_PAR RUN_CORE1))
	# Pad C0 binary with 0xFF
	$(OBJCOPY) --gap-fill 0xFF --pad-to $(CODE_SHIFT) --input-target binary --output-target binary ./C0/output/$(BUILD_TYPE)/C0.bin ./output/tmp0.bin
	# Append C1 binary
	cat ./output/tmp0.bin ./C1/output/$(BUILD_TYPE)/C1.bin > output/tmp_$(IMAGE_NAME)
	# Remove temporary files
	rm -f ./output/tmp0.bin
	sed '/^CODE_SHIFT /d' -i config.mk
else
	cp ./C0/output/$(BUILD_TYPE)/C0.bin ./output/tmp_$(IMAGE_NAME)
endif
ifeq ($(MEM_REG_ROM), QSPI1)
	unexport $(TRANSIT_PARAMS)
	make qspi_xip_loader
else
	mv ./output/tmp_$(IMAGE_NAME) ./output/$(IMAGE_NAME)
endif


ifeq ($(MEM_REG_ROM), QSPI1)
qspi_xip_loader:
	make -C $(XIP_LOADER_DIR) clean && make -C $(XIP_LOADER_DIR) MEM_REG_ROM=TCMA CLK_CCLK=$(CLK_CCLK) QSPI_CLK=$(QSPI_CLK) QSPI_APP_OFFSET=$(QSPI_APP_OFFSET)
	$(OBJCOPY) --gap-fill 0xFF --pad-to $(QSPI_APP_OFFSET) --input-target binary --output-target binary $(XIP_LOADER_DIR)/output/debug/QSPI_XIPLoader.bin ./output/tmp_xip_loader.bin
	cat ./output/tmp_xip_loader.bin ./output/tmp_$(IMAGE_NAME) > ./output/$(IMAGE_NAME)
	@rm -f ./output/tmp_xip_loader.bin ./output/tmp_$(IMAGE_NAME)
endif
