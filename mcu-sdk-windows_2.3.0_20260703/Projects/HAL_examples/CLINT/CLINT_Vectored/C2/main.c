/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/CLINT/CLINT_Vectored/C2/main.c
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
#include "bmcu_common.h"

volatile uint64_t Time;
volatile uint64_t TimeCmp;

int main(void)
{
    /* Get timer value */
    Time = __get_time();

    /* Set timer compare value */
    TimeCmp = Time + 1000000ULL;
    __set_timecmp(TimeCmp);

    /* Enable timer interrupt */
    CLINT_EnableIRQ(CLINT_MT_IRQn);

    /* Enable interrupts globally */
    __enable_irq();

    for(;;) {}

	return 0;
}

void __attribute__ ((interrupt)) Timer_IRQHandler(void)
{
    /* Get timer value */
    Time = __get_time();

    /* Set timer compare value */
    TimeCmp = Time + 1000000ULL;
    __set_timecmp(TimeCmp);

    /* Set CORE2 interrupt flag */
#if defined (BMCU_U)
    CORE2_INT->IER |= CORE2_INT_IER_EN;
#endif

#if defined (BE_U1000)
    CORE2_INT->IER |= CORE2_INT_IER_EN;
    __delay_cycles(10UL);
    CORE2_INT->IER &= ~CORE2_INT_IER_EN;
#endif
}
