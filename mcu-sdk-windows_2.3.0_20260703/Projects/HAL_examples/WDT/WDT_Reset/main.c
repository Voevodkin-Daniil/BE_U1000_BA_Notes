/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/WDT/WDT_Reset/main.c
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
#include <stdio.h>
#include "bmcu_cru.h"
#include "bmcu_wdt.h"
#include "bsp.h"

int
__io_putchar (int ch) {
    return bsp_serial_putchar(ch);
}

int
__io_getchar (void) {
    return -1;
}

/**
 * @brief Clock configuration:
 *
 * PLL   = 200MHz
 * CCLK  = 100MHz
 * PCLK0 = 100MHz
 * PCLK1 = 100MHz
 * PCLK2 = 100MHz
 * HCLK  = 100MHz
 */
void
CLK_Init (void) {

    CRU_PLL_BypassMode_Disable();

    CRU_PLL_SetSource(CRU_PLL_SRC_C0);

    CRU_PLL_ForceReset();
    CRU_PLL_Config(1UL, 48UL, 6UL, 48UL);
    CRU_PLL_ReleaseReset();

    __delay_cycles(500UL);

    CRU_SetCCLKPrescaler(CRU_CLK_DIV_2);
    CRU_CCLKPrescaler_Enable();
    CRU_SetCCLKSource(CRU_CLK_SRC_PLL);

    CRU_SetPCLK0Prescaler(CRU_CLK_DIV_2);
    CRU_PCLK0Prescaler_Enable();
    CRU_SetPCLK0Source(CRU_CLK_SRC_PLL);

    CRU_SetPCLK1Prescaler(CRU_CLK_DIV_2);
    CRU_PCLK1Prescaler_Enable();
    CRU_SetPCLK1Source(CRU_CLK_SRC_PLL);

    CRU_SetPCLK2Prescaler(CRU_CLK_DIV_2);
    CRU_PCLK2Prescaler_Enable();
    CRU_SetPCLK2Source(CRU_CLK_SRC_PLL);

    CRU_SetHCLKPrescaler(CRU_CLK_DIV_2);
    CRU_HCLKPrescaler_Enable();
    CRU_SetHCLKSource(CRU_CLK_SRC_PLL);

    /** Configure 1MHz clocks */
    CRU_SetCCLK1MHzPrescaler(99UL);
    CRU_SetHCLK1MHzPrescaler(99UL);
}

void
init (void) {
    /* Prepare eFlash to 100 MHz CCLK. See PLL configuration */
    EFLASH_Init(100000000UL);

    CLK_Init();

    bsp_serial_init();

    CRU_APB0_EnableClock(CRU_APB0_PERIPH_WDT0);
}

int
main (void) {
    init();
    printf("WDT reset example, %s %s\r\n", __DATE__, __TIME__);

    /* WDT reset does not disable it. It is a safety feature of the MCU.
       If necessary the WDT should be disabled manually. */
    printf("Disable WDT after reset\r\n");
    WDT_DeInit(WDT0);

    printf("Wait 10s to demonstrate that WDT has been disabled\r\n");
    __delay_ms(10000UL);

    printf("Start WDT\r\n");
    WDT_InitStruct_TypeDef WDT_InitStruct;
    WDT_StructInit(&WDT_InitStruct);
    WDT_InitStruct.Mode = WDT_MODE_RESET;
    WDT_InitStruct.Rstlen = WDT_RST_LEN_32;
    WDT_InitStruct.Timeout = WDT_TIME_PERIOD_15;
    WDT_Init(WDT0, &WDT_InitStruct);

    printf("Wait for WDT to reset MCU\r\n");
    for (;;) {
        /* Wait for WDT to reset MCU */
    }

    return 0;
}
