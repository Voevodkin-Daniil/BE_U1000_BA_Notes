# Core0 or Core1 (the same memory map for both cores)
TCMA_ORIGIN       := 0x40010000
TCMA_SIZE         := 64K

TCMB_ORIGIN       := 0x40020000
TCMB_SIZE         := 96K

FRONT_PORT_OFFSET := 0x10000000
FRONT_PORT_ORIGIN := 0x50000000
FRONT_PORT_SIZE   := 256K

SRAM_ORIGIN       := 0x70000000
SRAM_SIZE         := 32K

QSPI0_ORIGIN      := 0x80000000
QSPI0_SIZE        := 16M

QSPI1_ORIGIN      := 0x90000000
QSPI1_SIZE        := 16M

EFLASH_ORIGIN     := 0xA0000000
EFLASH_SIZE       := 256K

ifeq ($(CORE_NUM),CORE_2)
    TCMA_ORIGIN   := 0x40007800
    TCMA_SIZE     := 2K

    TCMB_ORIGIN   := 0x40008000
    TCMB_SIZE     := 8K

    # Dummy values for ld script
    # Core 2 can access only the self TCM memory
    SRAM_ORIGIN   := 0x00
    SRAM_SIZE     := 0
    QSPI0_ORIGIN  := 0x00
    QSPI0_SIZE    := 0
    QSPI1_ORIGIN  := 0x00
    QSPI1_SIZE    := 0
    EFLASH_ORIGIN := 0x00
    EFLASH_SIZE   := 0
endif
