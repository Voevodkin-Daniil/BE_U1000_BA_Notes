/**
 * *****************************************************************************
 *  @file       Projects/_template/module1/src/module1.c
 *  @author     Baikal electronics SDK team
 *  @brief      Template example source file
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
#include "bmcu_uart.h"
#include "module1.h"

int __io_putchar(int ch)
{
    while ((UART_GetLineStatus(UART0) & UART_LINE_STATUS_THRE) == 0UL);
    UART_TransmitData8b(UART0, (uint8_t)ch);

    return ch;
}


/*  Note: there is no UART configuration in this example.
    It will work only if the UART0 was preconfigured (by bootloader, for example)
*/
int
module1_func (void) {
    printf("Example project\r\n\tbuild %s, %s\r\n", __DATE__, __TIME__);
    printf("The quick brown fox jumps over the lazy dog\r\n");
    return CONST1;
}
