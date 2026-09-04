/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/UART/UART_printf/main.c
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
#include <stdio.h>
#include "bmcu_cru.h"
#include "bsp.h"

#define MSG_PRINT_PERIOD (1000U)


int
__io_putchar (int ch) {
    return bsp_serial_putchar(ch);
}


int
main (void) {
    /* Configure 1MHz clocks */
    CRU_SetCCLK1MHzPrescaler(24UL);
    
    /* Init serial interface */
    bsp_serial_init();

    for (;;) {
        printf("UART printf example\r\n");
        __delay_ms(MSG_PRINT_PERIOD);
    }

	return 0;
}
