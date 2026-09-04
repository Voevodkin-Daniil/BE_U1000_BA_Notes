/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/TIM/TIM_TimeBase/main.c
 *  @author     Baikal electronics SDK team
 *  @brief      HAL example source file
 *  @version    2.3.0
 *  @date       20260703
 * *****************************************************************************
 *  @copyright Copyright (c) 2026 Baikal Electronics JSC
 *
 *  This software is licensed under terms that can be found in the LICENSE file
 *  in the root directory of this software component.
 *  If no LICENSE file comes with this software, it is provided AS-IS.
 * *****************************************************************************
 */
#include <stdio.h>
#include "bmcu_cru.h"
#include "bmcu_tim.h"

int
__io_putchar (int ch) {
    return bsp_serial_putchar(ch);
}


void
TIM0_Init (void) {
    /* Enable TIM peripheral clock */
    CRU_APB0_EnableClock(CRU_APB0_PERIPH_TIM0);

    TIM_DeInit(TIM0, TIM_CH0);

    CRU_Clocks_TypeDef cru_clocks;
    CRU_GetSystemClocksFreq(&cru_clocks);

    /* Init timer channel */
    TIM_InitStruct_TypeDef TIM_InitStruct;
    TIM_StructInit(&TIM_InitStruct);
    TIM_InitStruct.CounterMode = TIM_COUNTERMODE_PERIODIC;
    TIM_InitStruct.LoadCount   = cru_clocks.PCLK0_Frequency - 1UL;
    TIM_Init(TIM0, TIM_CH0, &TIM_InitStruct);

    /* Enable timer interrupt */
    TIM_EnableIT(TIM0, TIM_CH0);

    /* Configure timer interrupt in CLIC */
    CLIC_ConfigIRQ(CLIC_TIM0_Channel0_IRQn,         /* Interrupt */
                   CLIC_INTATTR_MODE_MACHINE,       /* Privilege mode */
                   1U,                              /* Level */
                   1U,                              /* Priority */
                   CLIC_INTATTR_SHV_VECTORED,       /* Vector mode */
                   CLIC_INTATTR_TRIG_TYPE_LEVEL,    /* Type */
                   CLIC_INTATTR_TRIG_POL_P);        /* Polarity */

    CLIC_EnableIRQ(CLIC_TIM0_Channel0_IRQn);
}


void
Init (void) {
    /* Configure 1MHz clocks */
    CRU_SetCCLK1MHzPrescaler(24UL);

    /* CLIC */
    CLIC_Config(1U, 1U);
    CLIC_SetLevelThreshold(0U);
    
    /* Init serial interface */
    bsp_serial_init();

    /* LED */
    bsp_led_init();

    /* TIM */
    TIM0_Init();

    /* Enable interrupts globally */
    __enable_irq();
}


int
main (void) {
    Init();
    printf("TIM time base example, %s %s\r\n", __DATE__, __TIME__);

    /* Start timer */
    TIM_EnableChannel(TIM0, TIM_CH0);

    for (;;) {
    }

    return 0;
}


void __attribute__ ((interrupt))
TIM0_Channel0_IRQHandler (void) {
    bsp_led_toggle();

    /* Clear timer channel interrupt */
    TIM_ClearIT(TIM0, TIM_CH0);
}
