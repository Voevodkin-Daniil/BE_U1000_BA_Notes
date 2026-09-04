/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/TIM/TIM_PWMOutput/main.c
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
    /* Init TIM output pin */
    CRU_PIN_InitStruct_TypeDef CRU_PIN_InitStruct;
    CRU_PIN_StructInit(&CRU_PIN_InitStruct);
    CRU_PIN_InitStruct.Port          = CRU_PORT_A;
    CRU_PIN_InitStruct.Pin           = CRU_PIN_13;
    CRU_PIN_InitStruct.Pull          = CRU_PIN_PULL_NO;
    CRU_PIN_InitStruct.DriveStrength = CRU_PIN_DRIVE_STRENGTH_3;
    CRU_PIN_InitStruct.Alternate     = CRU_PIN_AF_5;
    CRU_PIN_Init(&CRU_PIN_InitStruct);

    /* Enable TIM peripheral clock */
    CRU_APB0_EnableClock(CRU_APB0_PERIPH_TIM0);
    
    TIM_DeInit(TIM0, TIM_CH1);
    
    CRU_Clocks_TypeDef cru_clocks;
    CRU_GetSystemClocksFreq(&cru_clocks);

    /* Init timer channel */
    TIM_InitStruct_TypeDef TIM_InitStruct;
    TIM_StructInit(&TIM_InitStruct);
    TIM_InitStruct.CounterMode      = TIM_COUNTERMODE_PERIODIC;
    TIM_InitStruct.PWMOutputCtrl    = ENABLE;
    TIM_InitStruct.PWM0N100ModeCtrl = DISABLE;
    TIM_InitStruct.LoadCount        = cru_clocks.PCLK0_Frequency / 100U - 1UL;  /* The value from which counting commences */
    TIM_InitStruct.LoadCount2       = cru_clocks.PCLK0_Frequency / 500U - 1UL;  /* The value when timer output changes from LOW to HIGH */
    TIM_Init(TIM0, TIM_CH1, &TIM_InitStruct);
    
    /* Enable timer channel */
    TIM_EnableChannel(TIM0, TIM_CH1);
}


void
Init (void) {
    /* Configure 1MHz clocks */
    CRU_SetCCLK1MHzPrescaler(24UL);
    
    /* Init serial interface */
    bsp_serial_init();

    /* TIM */
    TIM0_Init();
}


int
main (void) {
    Init();
    printf("TIM PWM output example, %s %s\r\n", __DATE__, __TIME__);

    for (;;) {
    }

    return 0;
}
