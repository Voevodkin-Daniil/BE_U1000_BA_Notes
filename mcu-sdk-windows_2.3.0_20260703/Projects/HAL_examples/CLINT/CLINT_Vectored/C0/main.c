/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/CLINT/CLINT_Vectored/C0/main.c
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
#include "bmcu_eflash.h"

#include "bsp.h"

#define CORE1_START_ADDRESS             (EFLASH_BASE + 0x00020000UL)
#define CORE2_START_ADDRESS             CORE2_TCMA_BASE

#define CORE2_IMAGE_START_ADDRESS       (EFLASH_BASE + 0x0003FC00UL)
#define CORE2_IMAGE_SIZE                1024UL

volatile uint32_t IntCnt;
uint32_t PrevIntCnt;

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
    EFLASH_Init(100000000);             /* 100 MHz. See PLL configuration */

    /* Init clock */
    CLK_Init();

    /* Init CLIC */
    CLIC_Config(1U, 1U);
    CLIC_SetLevelThreshold(0U);

    /* Configure CORE2 interrupt */
#if defined (BMCU_U)
    CLIC_ConfigIRQ(CLIC_CORE2_IRQn,                 /* Interrupt */
                   CLIC_INTATTR_MODE_MACHINE,       /* Privilege mode */
                   1U,                              /* Level */
                   1U,                              /* Priority */
                   CLIC_INTATTR_SHV_VECTORED,       /* Vector mode */
                   CLIC_INTATTR_TRIG_TYPE_LEVEL,    /* Type */
                   CLIC_INTATTR_TRIG_POL_P);        /* Polarity */
#endif

#if defined (BE_U1000)
    CLIC_ConfigIRQ(CLIC_CORE2_IRQn,                 /* Interrupt */
                   CLIC_INTATTR_MODE_MACHINE,       /* Privilege mode */
                   1U,                              /* Level */
                   1U,                              /* Priority */
                   CLIC_INTATTR_SHV_VECTORED,       /* Vector mode */
                   CLIC_INTATTR_TRIG_TYPE_EDGE,     /* Type */
                   CLIC_INTATTR_TRIG_POL_P);        /* Polarity */
#endif

    /* Enable CORE2 interrupt */
    CLIC_EnableIRQ(CLIC_CORE2_IRQn);

    /* Init serial interface */
    bsp_serial_init();

    /* Enable interrupts globally */
    __enable_irq();
}

int main(void)
{
    IntCnt = 0UL;
    PrevIntCnt = 0xFFFFFFFFUL;

    Init();

    printf("> Starting Core%lu..\r\n", __get_hart_id());

    /* Reset Core 1 */
    CRU_C1_ForceResetAll();

    __delay_ms(10UL);

    /* Start Core 1 */
    CRU_C1_Start(CORE1_START_ADDRESS);

    /* Delay for printf() on Core1 */
    __delay_ms(10UL);

    /* Reset Core 2 */
    CRU_C2_ForceResetAll();

    __delay_ms(100UL);

    /* Release TCM arbiter reset */
    CRU_C2_ReleaseResetCx();
    CRU_C2_ReleaseResetFP();

    __delay_ms(10UL);

    /* Copy Core 2 image to TCM memory */
    uint32_t data;
    for (uint32_t i = 0UL; i < (CORE2_IMAGE_SIZE / 4U); i++)
    {
        EFLASH_ReadWord(CORE2_IMAGE_START_ADDRESS + 4UL * i, &data, EFLASH_MAIN_ARRAY);
        *(__IO uint32_t*)(CORE2_TCMA_SYS_BASE + 4UL * i) = data;
    }

    /* Start Core 2 */
    CRU_C2_Start(CORE2_START_ADDRESS);

    for(;;)
    {
        if (IntCnt != PrevIntCnt)
        {
            /* Print the CORE2 interrupt count */
            printf("IntCnt = %lu\r\n", IntCnt);

            PrevIntCnt = IntCnt;
        }
    }

	return 0;
}

void __attribute__ ((interrupt)) CORE2_IRQHandler(void)
{
#if defined (BMCU_U)
    /* Clear the CORE2 interrupt flag */
    CORE2_INT_SYS->IER &= ~CORE2_INT_IER_EN;
#endif

    IntCnt++;
}
