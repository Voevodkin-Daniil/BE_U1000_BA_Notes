/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/EFLASH/EFLASH_MemoryTest/main.c
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

#define EFLASH_TEST_WORD                0xA5A5A5A5UL
#define EFLASH_START_ADDR               0xA0000000UL
#define EFLASH_WORD_CNT                 (256UL*1024UL/4UL)

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
    /* Init clock */
    CLK_Init();

    /* Init serial interface */
    bsp_serial_init();
}

EFLASH_Status_TypeDef EFLASH_MemoryTest(void)
{
    EFLASH_Status_TypeDef Status;
    uint32_t ErrorCnt;
    uint32_t Val;

    printf("EFLASH Erase Start\r\n");
    Status = EFLASH_EraseAll(EFLASH_MAIN_ARRAY);

    if (Status == EFLASH_COMPLETE)
    {
        printf("EFLASH Erase Ok\r\n");

        printf("EFLASH Test Start\r\n");
        ErrorCnt = 0UL;

        /* Write */
        for (uint32_t i = 0UL; i < EFLASH_WORD_CNT; i++)
        {
            Status = EFLASH_ProgramWord(EFLASH_START_ADDR + 4UL * i, EFLASH_TEST_WORD, EFLASH_MAIN_ARRAY);
            if (Status != EFLASH_COMPLETE)
                break;
        }

        if (Status == EFLASH_COMPLETE)
        {
            printf("EFLASH Program Ok\r\n");
        }
        else
        {
            printf("EFLASH Program Failed. Error = %d\r\n", Status);
        }

        /* Read */
        for (uint32_t i = 0UL; i < EFLASH_WORD_CNT; i++)
        {
            Status = EFLASH_ReadWord(EFLASH_START_ADDR + 4UL * i, &Val, EFLASH_MAIN_ARRAY);
            if (Status != EFLASH_COMPLETE)
                break;

            if (Val != EFLASH_TEST_WORD)
                ErrorCnt++;
        }

        if (Status == EFLASH_COMPLETE)
        {
            printf("EFLASH Read Complete. ErrorCnt = %ld\r\n", ErrorCnt);
        }
        else
        {
            printf("EFLASH Read Failed. Error = %d\r\n", Status);
        }
    }
    else
    {
        printf("EFLASH Erase Error = %d\r\n", Status);
    }

    return Status;
}

int main(void)
{
    EFLASH_Status_TypeDef Status;

    Status = EFLASH_Init(200000000);        /* 200 MHz. See PLL configuration */
    Init();
    printf("EFLASH Test Start\r\n");

    if (Status == EFLASH_COMPLETE)
    {
        printf("EFLASH Init Ok\r\n");
        EFLASH_MemoryTest();
    }
    else
    {
        printf("EFLASH Init Error = %d\r\n", Status);
    }

    printf("EFLASH Test End\r\n");

    for(;;) {}

	return 0;
}
