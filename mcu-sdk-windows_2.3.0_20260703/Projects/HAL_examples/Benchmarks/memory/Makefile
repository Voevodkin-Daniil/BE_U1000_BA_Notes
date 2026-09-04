BUILD_TYPE := debug

SDK_ROOT = $(realpath $(CURDIR)/../../../..)

# Toolchain
TC_PREFIX = $(SDK_ROOT)/Tools/toolchain/riscv32-none-elf/bin/riscv32-none-elf
OBJCOPY = $(TC_PREFIX)-objcopy

SHIFT = 0x20000 #EFLASH - 0x20000, TCMB - 0xC000

.DEFAULT_GOAL: all
.ONESHELL:
.PHONY : all clean dependents

# Targets
all: dependents cat

clean:
	cd C0 && $(MAKE) clean
	cd ..
	cd C1 && $(MAKE) clean
	cd ..
	rm -rf output

dependents:
	cd C0 && $(MAKE) clean && BUILD_TYPE=$(BUILD_TYPE) $(MAKE) all
	cd ..
	cd C1 && $(MAKE) clean && BUILD_TYPE=$(BUILD_TYPE) $(MAKE) all
	cd ..

cat:
    # Clean old image
	rm -rf output
	# Create output directory
	mkdir -p output
	# Pad C0 binary with 0xFF
	$(OBJCOPY) --gap-fill 0xFF --pad-to $(SHIFT) --input-target binary --output-target binary ./C0/output/$(BUILD_TYPE)/C0.bin ./output/tmp0.bin
	# Append C1 binary
	cat ./output/tmp0.bin ./C1/output/$(BUILD_TYPE)/C1.bin > output/fullimage.bin
	# Remove temporary files
	rm -f ./output/tmp0.bin
