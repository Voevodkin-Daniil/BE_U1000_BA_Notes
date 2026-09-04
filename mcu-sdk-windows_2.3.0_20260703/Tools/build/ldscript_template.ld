OUTPUT_ARCH(riscv)
OUTPUT_FORMAT(elf32-littleriscv)

ENTRY(_start)

MEMORY {
    ROM (#ROM_ACCESS_RIGHTS) : ORIGIN = #ROM_ORIGIN, LENGTH = #ROM_SIZE /* #ROM_COMMENT #DELLINE1*/
    RAM (#RAM_ACCESS_RIGHTS) : ORIGIN = #RAM_ORIGIN, LENGTH = #RAM_SIZE /* #RAM_COMMENT */
}

SECTIONS {
    .text : {
        /* Block header to be parsed by the bootROM */
        . = ORIGIN(#ROM_NAME);
        LONG(__HEADER_JUMP_OVER__) /* Jump over the header structure */
        . = ORIGIN(#ROM_NAME) + __HEADER_OFFSET_MAGIC_NUM__;
        LONG(__HEADER_MAGIC_NUM__)
        . = ORIGIN(#ROM_NAME) + __HEADER_OFFSET_COPY_ADDR__;
        LONG(0x00) /* Copy data from the begining of memory region */
        . = ORIGIN(#ROM_NAME) + __HEADER_OFFSET_COPY_SIZE__;
        LONG(__HEADER_COPY_SIZE__)
        . = ORIGIN(#ROM_NAME) + __HEADER_SIZE__;
        /* End of the block header */
        
        *(.text._start)  /* Reset vector */
        . = ALIGN(4);
        KEEP(*(.text.isr_vector))
        *(.text)
        *(.text.*)
    } > #ROM_NAME

    .note.gnu.build-id : ALIGN(4) {
        KEEP(*(.note.gnu.build-id))
    } > #ROM_NAME

    /* .rodata section must be located right before .data section */
    .rodata : ALIGN(4) {
        *(.srodata)
        *(.srodata.*)
        *(.rodata)
        *(.rodata.*)
    } #RODATA_IN_RAM_MOVE> #ROM_NAME  /* See RODATA_IN_RAM build parameter description */

    /* .data section must be located right after .rodata section */
	.data : ALIGN(4) {
        *(.data)
        *(.data.*)
        *(.sdata)
        *(.sdata.*)
    } > RAM AT> #ROM_NAME

    .bss (NOLOAD) : ALIGN(4) {
        *(.sbss)
        *(.sbss.*)
        *(.bss)
        *(.bss.*)
        *(COMMON)
    } > RAM

    .heap (NOLOAD) : ALIGN(4) {
        _heap_start = .;
    } > RAM
}

/* Variables for the header to be parsed by the bootROM */
__HEADER_OFFSET_MAGIC_NUM__ = 0x20;
__HEADER_OFFSET_COPY_ADDR__ = 0x28;
__HEADER_OFFSET_COPY_SIZE__ = 0x2C;
__HEADER_SIZE__             = 0x30;

__HEADER_JUMP_OVER__        = 0x0300006F; /* jal x0, 48 */
__HEADER_MAGIC_NUM__        = #IMAGE_MAGIC_NUM;
__HEADER_COPY_SIZE__        = LOADADDR(.data) + SIZEOF(.data) - LOADADDR(.text);

/* Variables for startup code */
__DATA_START__         = ADDR(#DATA_START);
__DATA_END__           = ADDR(.data) + SIZEOF(.data);
__DATA_IMAGE_START__   = LOADADDR(#DATA_START);
__BSS_START__          = ADDR(.bss);
__BSS_END__            = ADDR(.bss) + SIZEOF(.bss);
__SP                   = ORIGIN(RAM) + LENGTH(RAM);

/* "__global_pointer$" is a reserved GNU LD linker symbol.
   It serves as a special marker that indicates where
   the global pointer value should be placed in memory. */
__global_pointer$      = MIN(__DATA_START__ + 0x800,
                 MAX(__DATA_START__ + 0x800, __BSS_END__ - 0x800));
