/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/CLIC/CLIC_Vectored/main.c
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

#include "bsp.h"

volatile uint32_t Ready;
volatile uint64_t Time;
volatile uint64_t IntTime;

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

void Init(void)
{
    /* Prepare eFlash to 200 MHz CCLK. See PLL configuration */
    EFLASH_Init(200000000);
    
    /* Init clock */
    CLK_Init();

    /* Init CLIC */
    CLIC_Config(1U, 1U);
    CLIC_SetLevelThreshold(0U);

    /* Configure timer interrupt */
    CLIC_ConfigIRQ(CLIC_MT_IRQn,                    /* Interrupt */
                   CLIC_INTATTR_MODE_MACHINE,       /* Privilege mode */
                   1U,                              /* Level */
                   1U,                              /* Priority */
                   CLIC_INTATTR_SHV_VECTORED,       /* Vector mode */
                   CLIC_INTATTR_TRIG_TYPE_LEVEL,    /* Type */
                   CLIC_INTATTR_TRIG_POL_P);        /* Polarity */

    /* Init serial interface */
    bsp_serial_init();

    /* Enable interrupts globally */
    __enable_irq();
}

int main(void)
{
    Init();

    for(;;)
    {
        /* Reset ready flag */
        Ready = 0UL;

        /* Get timer value */
        Time = __get_time();

        printf("Timer value = 0x%08lX%08lX\r\n",
               (uint32_t)((Time >> 32) & 0xFFFFFFFFULL),
               (uint32_t)(Time & 0xFFFFFFFFULL));

        /* Set timer compare value */
        uint64_t CmpVal = Time + 1000000ULL;
        __set_timecmp(CmpVal);

        printf("Timer compare value = 0x%08lX%08lX\r\n",
               (uint32_t)((CmpVal >> 32) & 0xFFFFFFFFULL),
               (uint32_t)(CmpVal & 0xFFFFFFFFULL));

        /* Enable timer interrupt */
        CLIC_EnableIRQ(CLIC_MT_IRQn);

        /* Wait for timer interrupt */
        while (Ready == 0UL) {};

        printf("Timer interrupt time = 0x%08lX%08lX\r\n",
               (uint32_t)((IntTime >> 32) & 0xFFFFFFFFULL),
               (uint32_t)(IntTime & 0xFFFFFFFFULL));
    }

	return 0;
}

void __attribute__ ((interrupt)) Timer_IRQHandler(void)
{
    /* Save timer interrupt time */
    IntTime = __get_time();

    /* Set ready flag */
    Ready = 1UL;

    /* Disable timer interrupt */
    CLIC_DisableIRQ(CLIC_MT_IRQn);
}
