BUILD_TYPE := debug

SDK_ROOT = $(realpath $(CURDIR)/../../../..)

# Toolchain
TC_PREFIX = $(SDK_ROOT)/Tools/toolchain/riscv32-none-elf/bin/riscv32-none-elf
OBJCOPY = $(TC_PREFIX)-objcopy

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
	cd C2 && $(MAKE) clean
	cd ..
	rm -rf output

dependents:
	cd C0 && $(MAKE) clean && BUILD_TYPE=$(BUILD_TYPE) $(MAKE) all
	cd ..
	cd C1 && $(MAKE) clean && BUILD_TYPE=$(BUILD_TYPE) $(MAKE) all
	cd ..
	cd C2 && $(MAKE) clean && BUILD_TYPE=$(BUILD_TYPE) $(MAKE) all
	cd ..

cat:
    # Clean old image
	rm -rf output
    # Create output directory
	mkdir -p output
    # Pad C0 binary with 0xFF
	$(OBJCOPY) --gap-fill 0xFF --pad-to 0x20000 --input-target binary --output-target binary C0/output/$(BUILD_TYPE)/C0.bin output/tmp0.bin
    # Append C1 binary
	cat output/tmp0.bin C1/output/$(BUILD_TYPE)/C1.bin > output/tmp1.bin
    # Pad C0+C1 joined binary with 0xFF
	$(OBJCOPY) --gap-fill 0xFF --pad-to 0x3FC00 --input-target binary --output-target binary output/tmp1.bin output/tmp2.bin
    # Append C2 binary
	cat output/tmp2.bin C2/output/$(BUILD_TYPE)/C2.bin > output/tmp3.bin
    # Pad C0+C1+C2 joined binary with 0xFF
	$(OBJCOPY) --gap-fill 0xFF --pad-to 0x40000 --input-target binary --output-target binary output/tmp3.bin output/fullimage.bin
    # Remove temporary files
	rm -f output/tmp0.bin
	rm -f output/tmp1.bin
	rm -f output/tmp2.bin
	rm -f output/tmp3.bin
