/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/ADC/ADC_ScanMode_Single_TriggerSW_Temperature/main.c
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

/*  It is required to wait for the third End Of Conversion impulse during
    temperature measurement procedure */
#define RANKS_NUM (3U)
#define MSG_PERIOS_MS (1000U)

/*  If the ADC_CR1_TEMPSENS bit is set, ADC measures only the temperature value
    regardless of the channel configuration */
const struct {
    ADC_Rank_TypeDef     rank;
    ADC_Channels_TypeDef channel;
} rank_to_channel[RANKS_NUM] = {
    {.rank = ADC_RANK0, .channel = ADC_CH0},
    {.rank = ADC_RANK1, .channel = ADC_CH0},
    {.rank = ADC_RANK2, .channel = ADC_CH0}
};


int
__io_putchar (int ch) {
    return bsp_serial_putchar(ch);
}


void
CLK_Init (void) {
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


void
ADC0_Init (void) {
    /* Enable ADC0 clock */
    CRU_APB2_EnableClock(CRU_APB2_PERIPH_ADC0);

    /* Deinitialize ADC0 */
    ADC_DeInit(ADC0);

    /* Initialize ADC_InitStruct structure */
    ADC_InitStruct_TypeDef ADC_InitStruct;
    ADC_InitStruct.ConversionMode       = ADC_CONV_SINGLE;
    ADC_InitStruct.SequencerScanMode    = ADC_SEQ_SCAN_ENABLE;
    ADC_InitStruct.SequencerLength      = ADC_SEQ_SCAN_LENGTH_3RANKS;
    ADC_InitStruct.SequencerDiscontMode = ADC_SEQ_DISCONT_DISABLE;
    ADC_InitStruct.TriggerSource        = ADC_TRIG_SOFTWARE;
    ADC_InitStruct.DMATransfer          = DISABLE;

    /* Initialize ADC0 */
    ADC_Init(ADC0, &ADC_InitStruct);

    /* Set clock divider */
    ADC_SetClockDivision(ADC0, ADC_CLOCKDIVISION_DIV4);

    /* Configure ADC channel */
    for (size_t i = 0; i < RANKS_NUM; ++i) {
        ADC_SetSequencerRankChannel(ADC0, rank_to_channel[i].rank, rank_to_channel[i].channel);
#if defined (BMCU_U)
        ADC_SetChannelSamplingTime(ADC0, rank_to_channel[i].channel, ADC_SAMPLINGTIME_300CYCLES);
#elif defined (BE_U1000)
        ADC_SetChannelSamplingTime(ADC0, rank_to_channel[i].channel, ADC_SAMPLINGTIME_275CYCLES);
#endif
    }

    /* Enable ADC */
    ADC_Enable(ADC0);
}


int
main (void) {
    uint16_t d_0, d_1;

    /* Prepare eFlash to 100 MHz CCLK. See PLL configuration */
    EFLASH_Init(100000000UL);
    
    CLK_Init();
    bsp_serial_init();
    printf("MCU IC temperature measurement\r\n");
    
    ADC0_Init();
    /* Switch ADC to the temperature measurement mode */
    ADC_TS_Enable(ADC0);
    ADC_TS_SetFilterMode(ADC0, ADC_TS_MODE_AVG64);
    
    for (;;) {
        ADC_StartConversionSWStart(ADC0);
        ADC_StartCalibration(ADC0);
        ADC_TS_SetGain(ADC0, 0);
        
        /* Wait for the End Of Conversion (EOC) flag for each configured rank */
        for (size_t i = 0U; i < RANKS_NUM; i++) {
            /* Wait for the end of conversion and clean the corresponding flag */
            while (ADC_IsActiveFlag_EOC(ADC0) == 0UL);
            ADC_ClearFlag_EOC(ADC0);
        }

        d_0 = ADC_ReadConversionData12(ADC0);

        ADC_StartConversionSWStart(ADC0);
        ADC_StopCalibration(ADC0);
        ADC_TS_SetGain(ADC0, 0);

        for (size_t i = 0U; i < RANKS_NUM; i++) {
            while (ADC_IsActiveFlag_EOC(ADC0) == 0UL);
            ADC_ClearFlag_EOC(ADC0);
        }

        d_1 = ADC_ReadConversionData12(ADC0);

        printf("%f\n\r", (d_0 - d_1 - ADC_TEMPSENS_DINITIAL) * ADC_TEMPSENS_CE);
        __delay_ms(MSG_PERIOS_MS);
    }

	return 0;
}
