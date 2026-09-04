/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/PWMA/PWMA_InputCapture/main.c
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

#define PWMA_FREQ_NUM               5U  /* The number of output signal frequencies */

/* Output Compare modes */
static const uint32_t Frequency[PWMA_FREQ_NUM] = {
    1000,   /* 1kHz */
    5000,   /* 5kHz */
    10000,  /* 10kHz */
    20000,  /* 20kHz */
    25000   /* 50kHz */
};

/* Current frequency index */
static uint8_t FrequencyIndex = 0U;

/* The difference between the captured values */
static volatile uint16_t ICDiff = 0U;


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
PWMA1_Init (void)
{
    /* Init PWMA output pins */
    CRU_PIN_InitStruct_TypeDef CRU_PIN_InitStruct;
    CRU_PIN_StructInit(&CRU_PIN_InitStruct);
    CRU_PIN_InitStruct.Port          = CRU_PORT_B;
    CRU_PIN_InitStruct.Pin           = CRU_PIN_12;
    CRU_PIN_InitStruct.Pull          = CRU_PIN_PULL_NO;
    CRU_PIN_InitStruct.DriveStrength = CRU_PIN_DRIVE_STRENGTH_3;
    CRU_PIN_InitStruct.Alternate     = CRU_PIN_AF_4;
    CRU_PIN_Init(&CRU_PIN_InitStruct);

    /* Enable PWMA peripheral clock */
    CRU_APB2_EnableClock(CRU_APB2_PERIPH_PWMA1);

    PWMA_DeInit(PWMA1);

    CRU_Clocks_TypeDef cru_clocks;
    CRU_GetSystemClocksFreq(&cru_clocks);

    /* Init time base */
    PWMA_InitStruct_TypeDef PWMA_InitStruct;
    PWMA_StructInit(&PWMA_InitStruct);
    PWMA_InitStruct.Prescaler         = __PWMA_CALC_PSC(cru_clocks.PCLK2_Frequency, 500000UL);
    PWMA_InitStruct.CounterMode       = PWMA_COUNTERMODE_UP;
    PWMA_InitStruct.Autoreload        = __PWMA_CALC_ARR(cru_clocks.PCLK2_Frequency, PWMA_InitStruct.Prescaler, Frequency[FrequencyIndex]);
    PWMA_InitStruct.ClockDivision     = PWMA_CLOCKDIVISION_DIV1;
    PWMA_InitStruct.RepetitionCounter = 0U;
    PWMA_Init(PWMA1, &PWMA_InitStruct);

    /* Init Output Compare */
    PWMA_OC_InitStruct_TypeDef PWMA_OC_InitStruct;
    PWMA_OC_StructInit(&PWMA_OC_InitStruct);
    PWMA_OC_InitStruct.OCMode       = PWMA_OC_MODE_PWM1;
    PWMA_OC_InitStruct.OCState      = PWMA_OC_STATE_DISABLE;
    PWMA_OC_InitStruct.OCNState     = PWMA_OC_STATE_DISABLE;
    PWMA_OC_InitStruct.CompareValue = PWMA_GetAutoReload(PWMA1) / 2U;
    PWMA_OC_InitStruct.OCPolarity   = PWMA_OC_POLARITY_HIGH;
    PWMA_OC_InitStruct.OCNPolarity  = PWMA_OC_POLARITY_HIGH;
    PWMA_OC_InitStruct.OCIdleState  = PWMA_OC_IDLESTATE_LOW;
    PWMA_OC_InitStruct.OCNIdleState = PWMA_OC_IDLESTATE_LOW;
    PWMA_OC_Init(PWMA1, PWMA_CH2, &PWMA_OC_InitStruct);

    /* Enable Autoreload register preload */
    PWMA_EnableARRPreload(PWMA1);

    /* Enable Capture/Compare register preload */
    PWMA_OC_EnablePreload(PWMA1, PWMA_CH2);

    /* Main output enable */
    PWMA_EnableAllOutputs(PWMA1);

    /* Enable outputs */
    PWMA_CC_EnableChannel(PWMA1, PWMA_CH2);

    /* Enable counter */
    PWMA_EnableCounter(PWMA1);
}


void
PWMA3_Init (void)
{
    /* Init PWMA Input Capture pin */
    CRU_PIN_InitStruct_TypeDef CRU_PIN_InitStruct;
    CRU_PIN_StructInit(&CRU_PIN_InitStruct);
    CRU_PIN_InitStruct.Port          = CRU_PORT_C;
    CRU_PIN_InitStruct.Pin           = CRU_PIN_10;
    CRU_PIN_InitStruct.Pull          = CRU_PIN_PULL_DOWN;
    CRU_PIN_InitStruct.Alternate     = CRU_PIN_AF_5;
    CRU_PIN_Init(&CRU_PIN_InitStruct);

    /* Enable PWMA peripheral clock */
    CRU_APB2_EnableClock(CRU_APB2_PERIPH_PWMA3);

    PWMA_DeInit(PWMA3);

    CRU_Clocks_TypeDef cru_clocks;
    CRU_GetSystemClocksFreq(&cru_clocks);

    /* Init time base */
    PWMA_InitStruct_TypeDef PWMA_InitStruct;
    PWMA_StructInit(&PWMA_InitStruct);
    PWMA_InitStruct.Prescaler         = __PWMA_CALC_PSC(cru_clocks.PCLK2_Frequency, 25000000UL);
    PWMA_InitStruct.CounterMode       = PWMA_COUNTERMODE_UP;
    PWMA_InitStruct.Autoreload        = 0xFFFFU;
    PWMA_InitStruct.ClockDivision     = PWMA_CLOCKDIVISION_DIV1;
    PWMA_InitStruct.RepetitionCounter = 0U;
    PWMA_Init(PWMA3, &PWMA_InitStruct);

    /* Init Input Capture */
    PWMA_IC_InitStruct_TypeDef PWMA_IC_InitStruct;
    PWMA_IC_StructInit(&PWMA_IC_InitStruct);
    PWMA_IC_InitStruct.ICPolarity    = PWMA_IC_POLARITY_RISING;
    PWMA_IC_InitStruct.ICActiveInput = PWMA_IC_ACTIVEINPUT_DIRECTTI;
    PWMA_IC_InitStruct.ICPrescaler   = PWMA_IC_PRESCALER_DIV1;
    PWMA_IC_InitStruct.ICFilter      = PWMA_IC_FILTER_FDIV1;
    PWMA_IC_Init(PWMA3, PWMA_CH0, &PWMA_IC_InitStruct);

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

    /* Enable channel */
    PWMA_CC_EnableChannel(PWMA3, PWMA_CH0);

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

    /* User button */
    BTN_Init();

    /* PWM Output */
    PWMA1_Init();

    /* PWMA Input Capture */
    PWMA3_Init();

    /* Enable interrupts globally */
    __enable_irq();
}


int
main (void) {
    uint32_t Capture;

    init();
    printf("PWMA Input Capture example, %s %s\r\n", __DATE__, __TIME__);

    for (;;) {
        __delay_ms(1000UL);

        Capture = ICDiff;

        if (Capture != 0U)
        {
            /* The input signal frequency is calculated as follows:       */      
            /* Frequency = (CLK * ICPSC) / (Capture * (PSC + 1))          */
            /* where:                                                     */                                                          
            /* Capture is the difference between two consecutive captures */
            /* CLK is the base counter clock frequency                    */
            /* PSC is the base counter prescaler value                    */
            /* ICPSC is the Input Capture prescaler ratio                 */

            CRU_Clocks_TypeDef cru_clocks;
            CRU_GetSystemClocksFreq(&cru_clocks);

            /* Get the base counter clock frequency */
            uint32_t CLK = cru_clocks.PCLK2_Frequency;

            /* Get the base counter prescaler value */
            uint32_t PSC = PWMA_GetPrescaler(PWMA3);

            /* Get the Input Capture prescaler ratio */
            uint32_t ICPSC = __PWMA_GET_ICPSC_RATIO(PWMA_IC_GetPrescaler(PWMA3, PWMA_CH0));

            /* Calculate the input signal frequency */
            uint32_t MeasuredFrequency = (CLK * ICPSC) / (Capture * (PSC + 1U));

            printf("The input frequency is %luHz.\r\n", MeasuredFrequency);
        }
        else
        {
            printf("The measurement has failed.\r\n");
        }
    }

	return 0;
}


void __attribute__ ((interrupt))
GPIO2_IRQHandler (void) {
    GPIO_ClearPinIT(BSP_BTN_GPIO_PORT, BSP_BTN_GPIO_PIN);

    /* Set the new output signal frequency */
    FrequencyIndex = (FrequencyIndex + 1U) % PWMA_FREQ_NUM;

    CRU_Clocks_TypeDef cru_clocks;
    CRU_GetSystemClocksFreq(&cru_clocks);

    /* Calculate the new autoreload value */
    uint32_t Autoreload = __PWMA_CALC_ARR(cru_clocks.PCLK2_Frequency, PWMA_GetPrescaler(PWMA1), Frequency[FrequencyIndex]);

    /* Disable counter to make sure the autoreload and compare values are updated
       in the same cycle. Preload is enabled for both in initialization routine. */
    PWMA_DisableCounter(PWMA1);

    /* Update auto-reload value */
    PWMA_SetAutoReload(PWMA1, Autoreload);

    /* Update compare value */
    PWMA_OC_SetCompareCH2(PWMA1, Autoreload/2U);

    /* Re-enable counter */
    PWMA_EnableCounter(PWMA1);
}


void __attribute__ ((interrupt)) 
PWMA3_IRQHandler (void) {
    static uint16_t ICValue1 = 0U;
    static uint16_t ICValue2 = 0U;
    static uint8_t ICIndex = 0U;

    /* Capture/compare event */
    if (PWMA_IsActiveFlag_CC0(PWMA3))
    {
        if(ICIndex == 0U)
        {
            /* Capture the first value */
            ICValue1 = PWMA_IC_GetCaptureCH0(PWMA3);
            ICIndex++;
        }
        else
        {
            /* Capture the second value */
            ICValue2 = PWMA_IC_GetCaptureCH0(PWMA3);

            if (ICValue2 != ICValue1)
            {
                /* Update the difference between the captured values */
                ICDiff = ICValue2 - ICValue1;
            }
            else
            {
                /* Error */

                /* Invalidate the value */
                ICDiff = 0U;
            }
    
            ICIndex = 0U;    
        }

        /* Clear timer capture/compare interrupt flag */
        PWMA_ClearFlag_CC0(PWMA3);
    }

    /* Clear interrupt */
    PWMA_ClearIT(PWMA3);
}
