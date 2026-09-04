/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/CLINT/CLINT_Vectored/C1/main.c
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
#include <stdint.h>
#include <stdio.h>

#include "bmcu_common.h"

#include "bsp.h"

int __io_putchar(int ch)
{
    return bsp_serial_putchar(ch);
}

int __io_getchar(void)
{
    return -1;
}

int
main (void) {
    bsp_led_init();
    printf("> Starting Core%lu..\r\n", __get_hart_id());

    for (;;) {
        bsp_led_toggle();
        __delay_ms(500UL);
    }

	return 0;
}
