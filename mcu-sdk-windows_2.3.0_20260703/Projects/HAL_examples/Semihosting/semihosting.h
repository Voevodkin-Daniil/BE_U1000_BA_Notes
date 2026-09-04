/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/Semihosting/semihosting.h
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
#ifndef _SEMIHOSTING_H_
#define _SEMIHOSTING_H_

#include <stdint.h>
#include <stdarg.h>

enum semihosting_operation_numbers
{
	SH_SYS_OPEN = 0x01,
	SH_SYS_CLOSE = 0x02,
	SH_SYS_WRITEC = 0x03,
	SH_SYS_WRITE0 = 0x04,
	SH_SYS_WRITE = 0x05,
	SH_SYS_READC = 0x07
};

void sh_write0(const char *buf);

void sh_writec(char c);

char sh_readc(void);

#endif
