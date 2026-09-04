/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/ADC/ADC_DiscontMode_Single_TriggerSW/main.c
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
#include "bmcu_adc.h"
#include "bmcu_cru.h"

#include "bsp.h"

#define NUM_RANKS               8U

const ADC_Channels_TypeDef Ranks[NUM_RANKS] = {ADC_CH0, ADC_CH1, ADC_CH2, ADC_CH3, ADC_CH4, ADC_CH5, ADC_CH6, ADC_CH7};

int __io_putchar(int ch)
{
    return bsp_serial_putchar(ch);
}

int __io_getchar(void)
{
    return -1;
}

void CLK_Init(void)
{
    /*
     * Clock configuration:
     *
     * PLL = 200MHz
     * CCLK = 100MHz
     * PCLK0 = 100MHz
     * PCLK1 = 100MHz
     * PCLK2 = 100MHz
     * HCLK = 50MHz
     */

    CRU_PLL_BypassMode_Disable();

    /* Set PLL source clock to C0 */
    CRU_PLL_SetSource(CRU_PLL_SRC_C0);

    /* Configure PLL */
    CRU_PLL_ForceReset();
    CRU_PLL_Config(1UL, 48UL, 6UL, 48UL);
    CRU_PLL_ReleaseReset();

    /* PLL lock time min. 500 cycles */
    __delay_cycles(500UL);

    /* Configure CCLK */
    CRU_SetCCLKPrescaler(CRU_CLK_DIV_2);
    CRU_CCLKPrescaler_Enable();
    CRU_SetCCLKSource(CRU_CLK_SRC_PLL);

    /* Configure PCLK0 */
    CRU_SetPCLK0Prescaler(CRU_CLK_DIV_2);
    CRU_PCLK0Prescaler_Enable();
    CRU_SetPCLK0Source(CRU_CLK_SRC_PLL);

    /* Configure PCLK1 */
    CRU_SetPCLK1Prescaler(CRU_CLK_DIV_2);
    CRU_PCLK1Prescaler_Enable();
    CRU_SetPCLK1Source(CRU_CLK_SRC_PLL);

    /* Configure PCLK2 */
    CRU_SetPCLK2Prescaler(CRU_CLK_DIV_2);
    CRU_PCLK2Prescaler_Enable();
    CRU_SetPCLK2Source(CRU_CLK_SRC_PLL);

    /* Configure HCLK */
    CRU_SetHCLKPrescaler(CRU_CLK_DIV_4);
    CRU_HCLKPrescaler_Enable();
    CRU_SetHCLKSource(CRU_CLK_SRC_PLL);

    /* Configure 1MHz clocks */
    CRU_SetCCLK1MHzPrescaler(99UL);
    CRU_SetHCLK1MHzPrescaler(49UL);
}

void ADC0_Init(void)
{
    /* Enable ADC0 clock */
    CRU_APB2_EnableClock(CRU_APB2_PERIPH_ADC0);

    /* Deinitialize ADC0 */
    ADC_DeInit(ADC0);

    /* Initialize ADC_InitStruct structure */
    ADC_InitStruct_TypeDef ADC_InitStruct;
    ADC_InitStruct.ConversionMode = ADC_CONV_SINGLE;
    ADC_InitStruct.SequencerScanMode = ADC_SEQ_SCAN_ENABLE;
    ADC_InitStruct.SequencerLength = ADC_SEQ_SCAN_LENGTH_8RANKS;
    ADC_InitStruct.SequencerDiscontMode = ADC_SEQ_DISCONT_1RANK;
    ADC_InitStruct.TriggerSource = ADC_TRIG_SOFTWARE;
    ADC_InitStruct.DMATransfer = DISABLE;

    /* Initialize ADC0 */
    ADC_Init(ADC0, &ADC_InitStruct);

    /* Set clock divider */
    ADC_SetClockDivision(ADC0, ADC_CLOCKDIVISION_DIV4);

    /* Configure ADC channel */
    ADC_SetSequencerRankChannel(ADC0, ADC_RANK0, Ranks[0]);
    ADC_SetSequencerRankChannel(ADC0, ADC_RANK1, Ranks[1]);
    ADC_SetSequencerRankChannel(ADC0, ADC_RANK2, Ranks[2]);
    ADC_SetSequencerRankChannel(ADC0, ADC_RANK3, Ranks[3]);
    ADC_SetSequencerRankChannel(ADC0, ADC_RANK4, Ranks[4]);
    ADC_SetSequencerRankChannel(ADC0, ADC_RANK5, Ranks[5]);
    ADC_SetSequencerRankChannel(ADC0, ADC_RANK6, Ranks[6]);
    ADC_SetSequencerRankChannel(ADC0, ADC_RANK7, Ranks[7]);
#if defined (BMCU_U)
    ADC_SetChannelSamplingTime(ADC0, Ranks[0], ADC_SAMPLINGTIME_300CYCLES);
    ADC_SetChannelSamplingTime(ADC0, Ranks[1], ADC_SAMPLINGTIME_300CYCLES);
    ADC_SetChannelSamplingTime(ADC0, Ranks[2], ADC_SAMPLINGTIME_300CYCLES);
    ADC_SetChannelSamplingTime(ADC0, Ranks[3], ADC_SAMPLINGTIME_300CYCLES);
    ADC_SetChannelSamplingTime(ADC0, Ranks[4], ADC_SAMPLINGTIME_300CYCLES);
    ADC_SetChannelSamplingTime(ADC0, Ranks[5], ADC_SAMPLINGTIME_300CYCLES);
    ADC_SetChannelSamplingTime(ADC0, Ranks[6], ADC_SAMPLINGTIME_300CYCLES);
    ADC_SetChannelSamplingTime(ADC0, Ranks[7], ADC_SAMPLINGTIME_300CYCLES);
#elif defined (BE_U1000)
    ADC_SetChannelSamplingTime(ADC0, Ranks[0], ADC_SAMPLINGTIME_275CYCLES);
    ADC_SetChannelSamplingTime(ADC0, Ranks[1], ADC_SAMPLINGTIME_275CYCLES);
    ADC_SetChannelSamplingTime(ADC0, Ranks[2], ADC_SAMPLINGTIME_275CYCLES);
    ADC_SetChannelSamplingTime(ADC0, Ranks[3], ADC_SAMPLINGTIME_275CYCLES);
    ADC_SetChannelSamplingTime(ADC0, Ranks[4], ADC_SAMPLINGTIME_275CYCLES);
    ADC_SetChannelSamplingTime(ADC0, Ranks[5], ADC_SAMPLINGTIME_275CYCLES);
    ADC_SetChannelSamplingTime(ADC0, Ranks[6], ADC_SAMPLINGTIME_275CYCLES);
    ADC_SetChannelSamplingTime(ADC0, Ranks[7], ADC_SAMPLINGTIME_275CYCLES);
#endif

    /* Enable ADC */
    ADC_Enable(ADC0);
}

void Init(void)
{
    /* Prepare eFlash to 100 MHz CCLK. See PLL configuration */
    EFLASH_Init(100000000UL);
    
    /* Init clock */
    CLK_Init();

    /* Init serial interface */
    bsp_serial_init();

    /* Init ADC */
    ADC0_Init();
}

int main(void)
{
    uint16_t ConversionResult[NUM_RANKS];

    Init();

    for(;;)
    {
        /* Perform conversion for each configured rank */
        for (uint8_t i = 0U; i < NUM_RANKS; i++)
        {
            /* Clear EOC flag */
            ADC_ClearFlag_EOC(ADC0);

            /* Start conversion */
            ADC_StartConversionSWStart(ADC0);

            /* Wait for the end of conversion */
            while (ADC_IsActiveFlag_EOC(ADC0) == 0UL);

            /* Read conversion result */
            ConversionResult[i] = ADC_ReadConversionData12(ADC0);
        }

        /* Print conversion results */
        printf("Conversion results:");
        for (uint8_t i = 0U; i < NUM_RANKS; i++)
        {
            printf(" CH%d=%u", Ranks[i], ConversionResult[i]);
        }
        printf("\r\n");

        __delay_ms(1000UL);
    }

    return 0;
}
