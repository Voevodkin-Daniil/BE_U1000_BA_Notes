/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/Semihosting/semihosting.c
 *  @author     Baikal electronics SDK team
 *  @brief      HAL example source file
 *  @version    2.3.0
 *  @date       20260703
 * *****************************************************************************
 *  @copyright Copyright (c) 2025 Baikal Electronics JSC
 *
 *  This software is licensed under terms that can be found in the LICENSE file
 *  in the root directory of this software component.
 *  If no LICENSE file comes with this software, it is provided AS-IS.
 * *****************************************************************************
 */
#include "semihosting.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

/*REF: https://groups.google.com/a/groups.riscv.org/g/sw-dev/c/n-5VQ9PHZ4w/m/KbzH5t9MBgAJ */
static inline int __attribute__((always_inline)) sys_sh(int reason, void *argPack)
{
	register int value __asm__("a0") = reason;
	register void *ptr __asm__("a1") = argPack;
	__asm__ volatile(
		// Force 16-byte alignment to make sure that the 3 instructions fall
		// within the same virtual page.
		" .balign 16    \n"
		" .option push \n"
		// Force non-compressed RISC-V instructions
		" .option norvc \n"
		// semihosting e-break sequence
		" slli x0, x0, 0x1f \n" // # Entry NOP
		" ebreak \n"			// # Break to debugger
		" srai x0, x0,  0x7 \n" // # NOP encoding the semihosting call number 7
		" .option pop \n"
		/*mark (value) as an output operand*/
		: "=r"(value) /* Outputs */
		// The semihosting call number is passed in a0, and the argument in a1.
		: "0"(value), "r"(ptr) /* Inputs */
		// The "memory" clobber makes GCC assume that any memory may be arbitrarily read or written by the asm block,
		//  so will prevent the compiler from reordering loads or stores across it, or from caching memory values in registers across it.
		//  The "memory" clobber also prevents the compiler from removing the asm block as dead code.
		: "memory" /* Clobbers */
	);
	return value;
}

// function to write a NULL terminated string to the console
void sh_write0(const char *buf)
{
    // Print zero-terminated string
    sys_sh(SH_SYS_WRITE0, (void *)buf);
}

void sh_writec(char c)
{
    // Print zero-terminated string
    sys_sh(SH_SYS_WRITEC, (void *)&c);
}

char sh_readc(void)
{
    // Print zero-terminated string
    return sys_sh(SH_SYS_READC, (void *)NULL);
}
