/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/Benchmarks/whetstone/common.h
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
#include "bmcu_cru.h"
#include "bmcu_uart.h"

#include "clk.h"
#include "mutex.h"

#ifndef BOUDRATE
#define BOUDRATE 115200
#endif

extern uint32_t _start;
extern uint32_t __HEADER_SIZE__;
extern uint32_t __HEADER_MAGIC_NUM__;
extern char _sdata;

extern mutex_t* uart_mutex = (mutex_t*)MUTEX_ADDR;

int __io_putchar(int ch)
{
    while ((UART_GetLineStatus(UART0) & UART_LSR_THRE) == 0UL);
    UART_TransmitData8b(UART0, (uint8_t)ch);

    return ch;
}

int __io_getchar(void)
{
    return -1;
}

int main_part(void)
{
#if (OPT != 0) && (MEM_REG_ROM != 0)
    PRINTF("\r\n> main()=%p\r\n", main_part);
//    PRINTF("> Sections: _stext=%p, _srodata=%p, _sdata=%p\r\n", &text, &_srodata, &data);

    PRINTF("> Starting Coremark..\r\n");

    PRINTF("Build %s, %s\r\n", __DATE__, __TIME__);
    PRINTF("\r\n");

    CLK_Print();
#else
    PRINTF("> Starting Coremark..\r\n");
#endif

	coremark_main();

    while(1);

    return 0;
}
