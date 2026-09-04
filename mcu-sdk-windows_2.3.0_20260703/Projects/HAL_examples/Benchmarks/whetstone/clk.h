/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/Benchmarks/whetstone/source/clk.h
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
#ifndef _CLK_H
#define _CLK_H

#include <stdint.h>

void CLK_Init(void);
void CLK_Print(void);
uint32_t CLK_GetCCLKFreq(void);

#endif
