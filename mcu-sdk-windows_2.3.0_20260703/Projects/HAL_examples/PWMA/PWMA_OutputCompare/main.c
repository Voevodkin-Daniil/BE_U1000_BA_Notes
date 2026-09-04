/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/PWMA/PWMA_OutputCompare/main.c
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

#define PWMA_OC_MODE_NUM            3U  /* The number of Output Compare modes */

/* Output Compare modes */
static const PWMA_OC_Mode_TypeDef OCMode[PWMA_OC_MODE_NUM] = {
    PWMA_OC_MODE_TOGGLE,            /* OCyREF toggles on compare match */
    PWMA_OC_MODE_FORCED_INACTIVE,   /* OCyREF is forced low */
    PWMA_OC_MODE_FORCED_ACTIVE      /* OCyREF is forced high */
};

/* Current output compare mode */
static uint8_t OCModeIndex = 0U;


int
__io_putchar (int ch) {
    return bsp_serial_putchar(ch);
}


void
BTN_Init (void) {
    /* Initialize CRU settings of the user button pin */
    CRU_PIN_InitStruct_TypeDef CRU_PIN_InitStruct;
    CRU_PIN_StructInit(&CRU_PIN_InitStruct);
    CRU_PIN_InitStruct.Port = BSP_BTN_CRU_PORT;
    CRU_PIN_InitStruct.Pin = BSP_BTN_CRU_PIN;
    CRU_PIN_InitStruct.Pull = CRU_PIN_PULL_NO;
    CRU_PIN_Init(&CRU_PIN_InitStruct);

    /* Initialize GPIO settings of the user button pin */
    GPIO_InitStruct_TypeDef GPIO_InitStruct;
    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.PinMask = BSP_BTN_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
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
    /* Init PWMA output pins */
    CRU_PIN_InitStruct_TypeDef CRU_PIN_InitStruct;
    CRU_PIN_StructInit(&CRU_PIN_InitStruct);
    CRU_PIN_InitStruct.Port          = CRU_PORT_C;
    CRU_PIN_InitStruct.Pin           = CRU_PIN_10;
    CRU_PIN_InitStruct.Pull          = CRU_PIN_PULL_NO;
    CRU_PIN_InitStruct.DriveStrength = CRU_PIN_DRIVE_STRENGTH_3;
    CRU_PIN_InitStruct.Alternate     = CRU_PIN_AF_5;
    CRU_PIN_Init(&CRU_PIN_InitStruct);

    CRU_PIN_InitStruct.Port          = CRU_PORT_C;
    CRU_PIN_InitStruct.Pin           = CRU_PIN_9;
    CRU_PIN_Init(&CRU_PIN_InitStruct);

    /* Enable PWMA peripheral clock */
    CRU_APB2_EnableClock(CRU_APB2_PERIPH_PWMA3);

    PWMA_DeInit(PWMA3);

    CRU_Clocks_TypeDef cru_clocks;
    CRU_GetSystemClocksFreq(&cru_clocks);

    /* Init time base */
    PWMA_InitStruct_TypeDef PWMA_InitStruct;
    PWMA_StructInit(&PWMA_InitStruct);
    PWMA_InitStruct.Prescaler         = __PWMA_CALC_PSC(cru_clocks.PCLK2_Frequency, 1000U);
    PWMA_InitStruct.CounterMode       = PWMA_COUNTERMODE_UP;
    PWMA_InitStruct.Autoreload        = __PWMA_CALC_ARR(cru_clocks.PCLK2_Frequency, PWMA_InitStruct.Prescaler, 10U);
    PWMA_InitStruct.ClockDivision     = PWMA_CLOCKDIVISION_DIV1;
    PWMA_InitStruct.RepetitionCounter = 0U;
    PWMA_Init(PWMA3, &PWMA_InitStruct);

    /* Init Output Compare */
    PWMA_OC_InitStruct_TypeDef PWMA_OC_InitStruct;
    PWMA_OC_StructInit(&PWMA_OC_InitStruct);
    PWMA_OC_InitStruct.OCMode       = OCMode[OCModeIndex];
    PWMA_OC_InitStruct.OCState      = PWMA_OC_STATE_DISABLE;
    PWMA_OC_InitStruct.OCNState     = PWMA_OC_STATE_DISABLE;
    PWMA_OC_InitStruct.CompareValue = PWMA_GetAutoReload(PWMA3) / 2U;
    PWMA_OC_InitStruct.OCPolarity   = PWMA_OC_POLARITY_HIGH;
    PWMA_OC_InitStruct.OCNPolarity  = PWMA_OC_POLARITY_HIGH;
    PWMA_OC_InitStruct.OCIdleState  = PWMA_OC_IDLESTATE_LOW;
    PWMA_OC_InitStruct.OCNIdleState = PWMA_OC_IDLESTATE_LOW;
    PWMA_OC_Init(PWMA3, PWMA_CH0, &PWMA_OC_InitStruct);

    /* Enable capture/compare interrupt */
    PWMA_EnableIT_CC0(PWMA3);

    /* Configure timer interrupt in CLIC */
    CLIC_ConfigIRQ(CLIC_PWMA3_IRQn,                 /* Interrupt */
                   CLIC_INTATTR_MODE_MACHINE,       /* Privilege mode */
                   1U,                              /* Level */
                   1U,                              /* Priority */
                   CLIC_INTATTR_SHV_VECTORED,       /* Vector mode */
                   CLIC_INTATTR_TRIG_TYPE_LEVEL,    /* Type */
                   CLIC_INTATTR_TRIG_POL_P);        /* Polarity */

    CLIC_EnableIRQ(CLIC_PWMA3_IRQn);

    /* Main output enable */
    PWMA_EnableAllOutputs(PWMA3);

    /* Enable outputs */
    PWMA_CC_EnableChannel(PWMA3, PWMA_CH0|PWMA_CH0N);

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

    /* Init LED */
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
    printf("PWMA Output Compare example, %s %s\r\n", __DATE__, __TIME__);

    for (;;) {
    }

	return 0;
}


void __attribute__ ((interrupt))
GPIO2_IRQHandler (void) {
    GPIO_ClearPinIT(BSP_BTN_GPIO_PORT, BSP_BTN_GPIO_PIN);

    /* Set the new Output Compare mode */
    OCModeIndex = (OCModeIndex + 1U) % PWMA_OC_MODE_NUM;

    /* Disable counter */
    PWMA_DisableCounter(PWMA3);
  
    /* Reset counter */
    PWMA_SetCounter(PWMA3, 0U);
  
    /* Set counter mode */
    PWMA_OC_SetMode(PWMA3, PWMA_CH0, OCMode[OCModeIndex]);

    if (OCMode[OCModeIndex] == PWMA_OC_MODE_FORCED_INACTIVE)
    {
        /* Forced inactive mode */

        /* Switch LED off */
        bsp_led_off();
    }
    else if (OCMode[OCModeIndex] == PWMA_OC_MODE_FORCED_ACTIVE)
    {
        /* Forced active mode */

        /* Switch LED on */
        bsp_led_on();
    }
    else
    {
        /* Toggle mode, the LED is controlled in the timer ISR */
    }
  
    /* Enable counter */
    PWMA_EnableCounter(PWMA3);
}


void __attribute__ ((interrupt)) 
PWMA3_IRQHandler (void) {
    if (PWMA_IsActiveFlag_CC0(PWMA3))
    {
        if (OCMode[OCModeIndex] == PWMA_OC_MODE_TOGGLE)
        {
            /* Toggle LED */
            bsp_led_toggle();
        }

        /* Clear timer capture/compare interrupt flag */
        PWMA_ClearFlag_CC0(PWMA3);
    }

    /* Clear interrupt */
    PWMA_ClearIT(PWMA3);
}
