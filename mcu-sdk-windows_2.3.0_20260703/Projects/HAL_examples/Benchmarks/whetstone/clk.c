/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/Benchmarks/whetstone/source/clk.c
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
#include "bmcu_qspi.h"

#include "clk.h"

#ifndef CLK_CCLK
#define CLK_CCLK        200
#endif

void CLK_Init()
{
    /*
     * REF_CLK=25MHz, PLL=200 MHz, CPU=200MHz, APB0, APB1, APB2, AHB=100MHz
     *
     * Clock configuration:
     *
     * PLL = 200MHz
     * CCLK = 200MHz
     * PCLK0 = 100MHz
     * PCLK1 = 100MHz
     * PCLK2 = 100MHz
     * HCLK = 100MHz
     */

    CRU_PLL_BypassMode_Disable();

    /* Set PLL source clock to C0 */
    CRU_PLL_SetSource(CRU_PLL_SRC_C0);

    /* Configure PLL to 200 MHz */
    CRU_PLL_ForceReset();
    CRU_PLL_Config(1UL, 48UL, 6UL, 48UL);
    CRU_PLL_ReleaseReset();

    /* PLL lock time min. 500 cycles */
    __delay_cycles(500UL);

    /* Configure CCLK */
#if CLK_CCLK == 200
    CRU_SetCCLKPrescaler(CRU_CLK_DIV_1);
#elif CLK_CCLK == 100
    CRU_SetCCLKPrescaler(CRU_CLK_DIV_2);
#elif CLK_CCLK == 25
    CRU_SetCCLKPrescaler(CRU_CLK_DIV_8);
#else
    #error CLK_CCLK is not supported
#endif

    CRU_CCLKPrescaler_Enable();
    CRU_SetCCLKSource(CRU_CLK_SRC_PLL);

    /* Configure PCLK0 to 100 MHz */
    CRU_SetPCLK0Prescaler(CRU_CLK_DIV_2);
    CRU_PCLK0Prescaler_Enable();
    CRU_SetPCLK0Source(CRU_CLK_SRC_PLL);

    /* Configure PCLK1 to 100 MHz */
    CRU_SetPCLK1Prescaler(CRU_CLK_DIV_2);
    CRU_PCLK1Prescaler_Enable();
    CRU_SetPCLK1Source(CRU_CLK_SRC_PLL);

    /* Configure PCLK2 to 100 MHz */
    CRU_SetPCLK2Prescaler(CRU_CLK_DIV_2);
    CRU_PCLK2Prescaler_Enable();
    CRU_SetPCLK2Source(CRU_CLK_SRC_PLL);

    /* Configure HCLK to 100 MHz */
    CRU_SetHCLKPrescaler(CRU_CLK_DIV_2);
    CRU_HCLKPrescaler_Enable();
    CRU_SetHCLKSource(CRU_CLK_SRC_PLL);

    /* Configure 1MHz clocks */
#if CLK_CCLK == 200
    CRU_SetCCLK1MHzPrescaler(199UL);
#elif CLK_CCLK == 100
    CRU_SetCCLK1MHzPrescaler(99UL);
#elif CLK_CCLK == 25
    CRU_SetCCLK1MHzPrescaler(24UL);
#else
    #error CLK_CCLK is not supported
#endif

    CRU_SetHCLK1MHzPrescaler(99UL);
}

void CLK_Print()
{
    CRU_Clocks_TypeDef cru_clocks;
    CRU_GetSystemClocksFreq(&cru_clocks);
    PRINTF("CCLK_Frequency: %lu\r\n", cru_clocks.CCLK_Frequency);
    PRINTF("CCLK_1MHzPrescaler: %lu\r\n", CRU_GetCCLK1MHzPrescaler());
    PRINTF("PCLK0_Frequency: %lu\r\n", cru_clocks.PCLK0_Frequency);
    PRINTF("PCLK1_Frequency: %lu\r\n", cru_clocks.PCLK1_Frequency);
    PRINTF("PCLK2_Frequency: %lu\r\n", cru_clocks.PCLK2_Frequency);
    PRINTF("HCLK_Frequency: %lu\r\n", cru_clocks.HCLK_Frequency);
    PRINTF("TCLK_Frequency: %lu\r\n", cru_clocks.TCLK_Frequency);
    PRINTF("CANx2CLK_Frequency: %lu\r\n", cru_clocks.CANx2CLK_Frequency);
    PRINTF("\r\n");
}

uint32_t CLK_GetCCLKFreq()
{
    CRU_Clocks_TypeDef cru_clocks;
    CRU_GetSystemClocksFreq(&cru_clocks);
    return cru_clocks.CCLK_Frequency;
}
