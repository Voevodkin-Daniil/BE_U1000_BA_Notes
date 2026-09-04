/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/GPIO/GPIO_LEDBlink/main.c
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
#include "bmcu_common.h"

int
main (void) {
    bsp_led_init();

    for (;;) {
        bsp_led_toggle();
        __delay_ms(500UL);
    }

	return 0;
}
