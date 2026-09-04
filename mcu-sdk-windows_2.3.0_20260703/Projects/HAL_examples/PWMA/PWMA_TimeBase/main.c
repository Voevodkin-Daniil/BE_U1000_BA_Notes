/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/PWMA/PWMA_TimeBase/main.c
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
#include "bmcu_gpio.h"
#include "bmcu_pwma.h"
#include "bsp.h"

#define PWMA_FREQ_NUM               5U

static const uint16_t AutoreloadValues[PWMA_FREQ_NUM] = {
    199U,   /* 200ms */
    399U,   /* 400ms */
    599U,   /* 600ms */
    799U,   /* 800ms */
    999U    /* 1000ms */
};

static uint8_t AutoreloadIndex = PWMA_FREQ_NUM - 1U;


int
__io_putchar (int ch) {
    return bsp_serial_putchar(ch);
}


void
BTN_Init (void) {
    /* Initialize CRU settings of the user button pin */
    CRU_PIN_InitStruct_TypeDef CRU_PIN_InitStruct;
    CRU_PIN_StructInit(&CRU_PIN_InitStruct);
    CRU_PIN_InitStruct.Port          = BSP_BTN_CRU_PORT;
    CRU_PIN_InitStruct.Pin           = BSP_BTN_CRU_PIN;
    CRU_PIN_InitStruct.Pull          = CRU_PIN_PULL_NO;
    CRU_PIN_Init(&CRU_PIN_InitStruct);

    /* Initialize GPIO settings of the user button pin */
    GPIO_InitStruct_TypeDef GPIO_InitStruct;
    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.PinMask = BSP_BTN_GPIO_PIN;
    GPIO_InitStruct.Mode    = GPIO_MODE_INPUT;
    GPIO_Init(BSP_BTN_GPIO_PORT, &GPIO_InitStruct);

    /* Configure interrupt for the user button GPIO pin */
    GPIO_SetPinITType(BSP_BTN_GPIO_PORT, BSP_BTN_GPIO_PIN, GPIO_IT_TYPE_EDGE);
    GPIO_SetPinBothEdgeIT(BSP_BTN_GPIO_PORT, BSP_BTN_GPIO_PIN, GPIO_IT_BOTHEDGE_DISABLE);
    GPIO_SetPinITPolarity(BSP_BTN_GPIO_PORT, BSP_BTN_GPIO_PIN, GPIO_IT_POL_LOW);
    GPIO_SetPinDebounce(BSP_BTN_GPIO_PORT, BSP_BTN_GPIO_PIN, GPIO_IT_DEBOUNCE_ENABLE);
    GPIO_SetPinIT(BSP_BTN_GPIO_PORT, BSP_BTN_GPIO_PIN, GPIO_IT_ENABLE);

    /* Configure CLIC for the GPIO interrupt */
    CLIC_ConfigIRQ(CLIC_GPIO2_IRQn,                 /* Interrupt */
                   CLIC_INTATTR_MODE_MACHINE,       /* Privilege mode */
                   1U,                              /* Level */
                   1U,                              /* Priority */
                   CLIC_INTATTR_SHV_VECTORED,       /* Vector mode */
                   CLIC_INTATTR_TRIG_TYPE_LEVEL,    /* Type */
                   CLIC_INTATTR_TRIG_POL_P);        /* Polarity */

    /* Enable GPIO interrupt */
    CLIC_EnableIRQ(CLIC_GPIO2_IRQn);
}


void
PWMA3_Init (void)
{
    /* Enable PWMA clock */
    CRU_APB2_EnableClock(CRU_APB2_PERIPH_PWMA3);

    PWMA_DeInit(PWMA3);

    CRU_Clocks_TypeDef cru_clocks;
    CRU_GetSystemClocksFreq(&cru_clocks);

    /* Init timer */
    PWMA_InitStruct_TypeDef PWMA_InitStruct;
    PWMA_StructInit(&PWMA_InitStruct);
    PWMA_InitStruct.Prescaler         = __PWMA_CALC_PSC(cru_clocks.PCLK2_Frequency, 1000U);
    PWMA_InitStruct.CounterMode       = PWMA_COUNTERMODE_UP;
    PWMA_InitStruct.Autoreload        = AutoreloadValues[AutoreloadIndex];
    PWMA_InitStruct.ClockDivision     = PWMA_CLOCKDIVISION_DIV1;
    PWMA_InitStruct.RepetitionCounter = 0U;
    PWMA_Init(PWMA3, &PWMA_InitStruct);

    /* Enable update interrupt */
    PWMA_EnableIT_UPDATE(PWMA3);

    /* Configure timer interrupt in CLIC */
    CLIC_ConfigIRQ(CLIC_PWMA3_IRQn,                 /* Interrupt */
                   CLIC_INTATTR_MODE_MACHINE,       /* Privilege mode */
                   1U,                              /* Level */
                   1U,                              /* Priority */
                   CLIC_INTATTR_SHV_VECTORED,       /* Vector mode */
                   CLIC_INTATTR_TRIG_TYPE_LEVEL,    /* Type */
                   CLIC_INTATTR_TRIG_POL_P);        /* Polarity */

    CLIC_EnableIRQ(CLIC_PWMA3_IRQn);

    /* Enable counter */
    PWMA_EnableCounter(PWMA3);
}


void
init (void) {
    /* Configure 1MHz clocks */
    CRU_SetCCLK1MHzPrescaler(24UL);

    /* CLIC */
    CLIC_Config(1U, 1U);
    CLIC_SetLevelThreshold(0U);
    
    /* Init serial interface */
    bsp_serial_init();

    /* LED */
    bsp_led_init();

    /* User button */
    BTN_Init();

    /* PWMA */
    PWMA3_Init();

    /* Enable interrupts globally */
    __enable_irq();
}


int
main (void) {
    init();
    printf("PWMA time base example, %s %s\r\n", __DATE__, __TIME__);

    for (;;) {
    }

	return 0;
}


void __attribute__ ((interrupt))
GPIO2_IRQHandler (void) {
    GPIO_ClearPinIT(BSP_BTN_GPIO_PORT, BSP_BTN_GPIO_PIN);

    /* Change the timer period by modifying the autoreload value */
    AutoreloadIndex++;
    if (AutoreloadIndex >= PWMA_FREQ_NUM)
        AutoreloadIndex = 0U;

    PWMA_SetAutoReload(PWMA3, AutoreloadValues[AutoreloadIndex]);
}


void __attribute__ ((interrupt)) 
PWMA3_IRQHandler (void) {
    if (PWMA_IsActiveFlag_UPDATE(PWMA3))
    {
        /* Toggle LED */
        bsp_led_toggle();

        /* Clear timer update interrupt flag */
        PWMA_ClearFlag_UPDATE(PWMA3);
    }

    /* Clear interrupt */
    PWMA_ClearIT(PWMA3);
}
