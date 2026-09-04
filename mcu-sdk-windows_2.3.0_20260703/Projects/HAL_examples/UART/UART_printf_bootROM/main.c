/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/UART/UART_pinrtf_bootROM/main.c
 *  @author     Baikal electronics SDK team
 *  @brief      UART printf with bootROM functions call example source file
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

#include "bmcu_common.h"
#define CPU_FREQ  (25000000U)
#define UART_BAUD (115200U)

/* See bootROM documentation for more info */
int
main (void) {
    uint32_t i = 0;
    // UART init
    ((uint32_t (*)(uintptr_t, uint32_t, uint32_t)) (0x4000636c))(UART0_BASE, CPU_FREQ, UART_BAUD);

    for (;;) {
        // printf
        ((int (*)(const char *, ...)) (0x4000b082))("%d UART bootROM printf example\r\n", i++);
        // ticks delay
        ((void (*)(uint32_t)) (0x40002f2c))(CPU_FREQ);
    }

	return 0;
}
