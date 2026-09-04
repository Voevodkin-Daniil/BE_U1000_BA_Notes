/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/PWMG/PWMG_InputCapture/main.c
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
#include "bmcu_pwmg.h"
#include "bsp.h"

#define PWMG_FREQ_NUM               5U  /* The number of output signal frequencies */

/* Output Compare modes */
static const uint32_t Frequency[PWMG_FREQ_NUM] = {
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
PWMG0_Init (void)
{
    /* Init PWMG output pins */
    CRU_PIN_InitStruct_TypeDef CRU_PIN_InitStruct;
    CRU_PIN_StructInit(&CRU_PIN_InitStruct);
    CRU_PIN_InitStruct.Port          = CRU_PORT_A;
    CRU_PIN_InitStruct.Pin           = CRU_PIN_15;
    CRU_PIN_InitStruct.Pull          = CRU_PIN_PULL_NO;
    CRU_PIN_InitStruct.DriveStrength = CRU_PIN_DRIVE_STRENGTH_3;
    CRU_PIN_InitStruct.Alternate     = CRU_PIN_AF_4;
    CRU_PIN_Init(&CRU_PIN_InitStruct);

    /* Enable PWMG peripheral clock */
    CRU_APB0_EnableClock(CRU_APB0_PERIPH_PWMG0);

    PWMG_DeInit(PWMG0);

    CRU_Clocks_TypeDef cru_clocks;
    CRU_GetSystemClocksFreq(&cru_clocks);

    /* Init time base */
    PWMG_InitStruct_TypeDef PWMG_InitStruct;
    PWMG_StructInit(&PWMG_InitStruct);
    PWMG_InitStruct.Prescaler     = __PWMG_CALC_PSC(cru_clocks.PCLK2_Frequency, 500000UL);
    PWMG_InitStruct.Autoreload    = __PWMG_CALC_ARR(cru_clocks.PCLK2_Frequency, PWMG_InitStruct.Prescaler, Frequency[FrequencyIndex]);
    PWMG_InitStruct.ClockDivision = PWMG_CLOCKDIVISION_DIV1;
    PWMG_Init(PWMG0, &PWMG_InitStruct);

    /* Init Output Compare */
    PWMG_OC_InitStruct_TypeDef PWMG_OC_InitStruct;
    PWMG_OC_StructInit(&PWMG_OC_InitStruct);
    PWMG_OC_InitStruct.OCMode       = PWMG_OC_MODE_PWM1;
    PWMG_OC_InitStruct.OCPolarity   = PWMG_OC_POLARITY_HIGH;
    PWMG_OC_InitStruct.OCState      = PWMG_OC_STATE_DISABLE;
    PWMG_OC_InitStruct.CompareValue = PWMG_GetAutoReload(PWMG0) / 2U;
    PWMG_OC_Init(PWMG0, &PWMG_OC_InitStruct);

    /* Enable Autoreload register preload */
    PWMG_EnableARRPreload(PWMG0);

    /* Enable Capture/Compare register preload */
    PWMG_OC_EnablePreload(PWMG0);

    /* Enable channel */
    PWMG_CC_EnableChannel(PWMG0);

    /* Enable counter */
    PWMG_EnableCounter(PWMG0);
}


void
PWMG1_Init (void)
{
    /* Init PWMG Input Capture pin */
    CRU_PIN_InitStruct_TypeDef CRU_PIN_InitStruct;
    CRU_PIN_StructInit(&CRU_PIN_InitStruct);
    CRU_PIN_InitStruct.Port          = CRU_PORT_B;
    CRU_PIN_InitStruct.Pin           = CRU_PIN_15;
    CRU_PIN_InitStruct.Pull          = CRU_PIN_PULL_DOWN;
    CRU_PIN_InitStruct.Alternate     = CRU_PIN_AF_4;
    CRU_PIN_Init(&CRU_PIN_InitStruct);

    /* Enable PWMG peripheral clock */
    CRU_APB1_EnableClock(CRU_APB1_PERIPH_PWMG1);

    PWMG_DeInit(PWMG1);

    CRU_Clocks_TypeDef cru_clocks;
    CRU_GetSystemClocksFreq(&cru_clocks);

    /* Init time base */
    PWMG_InitStruct_TypeDef PWMG_InitStruct;
    PWMG_StructInit(&PWMG_InitStruct);
    PWMG_InitStruct.Prescaler         = __PWMG_CALC_PSC(cru_clocks.PCLK2_Frequency, 25000000UL);
    PWMG_InitStruct.Autoreload        = 0xFFFFU;
    PWMG_InitStruct.ClockDivision     = PWMG_CLOCKDIVISION_DIV1;
    PWMG_Init(PWMG1, &PWMG_InitStruct);

    /* Init Input Capture */
    PWMG_IC_InitStruct_TypeDef PWMG_IC_InitStruct;
    PWMG_IC_StructInit(&PWMG_IC_InitStruct);
    PWMG_IC_InitStruct.ICPrescaler = PWMG_IC_PRESCALER_DIV1;
    PWMG_IC_InitStruct.ICFilter    = PWMG_IC_FILTER_FDIV1;
    PWMG_IC_InitStruct.ICPolarity  = PWMG_IC_POLARITY_RISING;
    PWMG_IC_Init(PWMG1, &PWMG_IC_InitStruct);

    /* Enable capture/compare interrupt */
    PWMG_EnableIT_CC0(PWMG1);

    /* Configure timer interrupt in CLIC */
    CLIC_ConfigIRQ(CLIC_PWMG1_IRQn,                 /* Interrupt */
                   CLIC_INTATTR_MODE_MACHINE,       /* Privilege mode */
                   1U,                              /* Level */
                   1U,                              /* Priority */
                   CLIC_INTATTR_SHV_VECTORED,       /* Vector mode */
                   CLIC_INTATTR_TRIG_TYPE_LEVEL,    /* Type */
                   CLIC_INTATTR_TRIG_POL_P);        /* Polarity */

    CLIC_EnableIRQ(CLIC_PWMG1_IRQn);

    /* Enable channel */
    PWMG_CC_EnableChannel(PWMG1);

    /* Enable counter */
    PWMG_EnableCounter(PWMG1);
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
    PWMG0_Init();

    /* PWMG Input Capture */
    PWMG1_Init();

    /* Enable interrupts globally */
    __enable_irq();
}


int
main (void) {
    uint32_t Capture;

    init();
    printf("PWMG Input Capture example, %s %s\r\n", __DATE__, __TIME__);

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
            uint32_t PSC = PWMG_GetPrescaler(PWMG1);

            /* Get the Input Capture prescaler ratio */
            uint32_t ICPSC = __PWMG_GET_ICPSC_RATIO(PWMG_IC_GetPrescaler(PWMG1));

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
    FrequencyIndex = (FrequencyIndex + 1U) % PWMG_FREQ_NUM;

    CRU_Clocks_TypeDef cru_clocks;
    CRU_GetSystemClocksFreq(&cru_clocks);

    /* Calculate the new autoreload value */
    uint32_t Autoreload = __PWMG_CALC_ARR(cru_clocks.PCLK2_Frequency, PWMG_GetPrescaler(PWMG0), Frequency[FrequencyIndex]);

    /* Disable counter to make sure the autoreload and compare values are updated
       in the same cycle. Preload is enabled for both in initialization routine. */
    PWMG_DisableCounter(PWMG0);

    /* Update auto-reload value */
    PWMG_SetAutoReload(PWMG0, Autoreload);

    /* Update compare value */
    PWMG_OC_SetCompare(PWMG0, Autoreload/2U);

    /* Re-enable counter */
    PWMG_EnableCounter(PWMG0);
}


void __attribute__ ((interrupt)) 
PWMG1_IRQHandler (void) {
    static uint16_t ICValue1 = 0U;
    static uint16_t ICValue2 = 0U;
    static uint8_t ICIndex = 0U;

    /* Capture/compare event */
    if (PWMG_IsActiveFlag_CC0(PWMG1))
    {
        if(ICIndex == 0U)
        {
            /* Capture the first value */
            ICValue1 = PWMG_IC_GetCapture(PWMG1);
            ICIndex++;
        }
        else
        {
            /* Capture the second value */
            ICValue2 = PWMG_IC_GetCapture(PWMG1);

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
        PWMG_ClearFlag_CC0(PWMG1);
    }

    /* Clear interrupt */
    PWMG_ClearIT(PWMG1);
}
