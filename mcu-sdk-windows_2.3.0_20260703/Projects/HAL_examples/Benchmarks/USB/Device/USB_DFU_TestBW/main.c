/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/Benchmarks/USB/Device/USB_DFU_TestBW/main.c
 *  @author     Baikal electronics SDK team
 *  @brief      USB DFU class example source file
 *  @version    2.3.0
 *  @date       20260703
 * 
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
#include "dfu.h"
#include "bsp.h"

int
__io_putchar (int ch) {
    return bsp_serial_putchar(ch);
}

void
CLK_Init (void) {
    CRU_PLL_BypassMode_Disable();
    
    /** Set PLL source clock to C0 */
    CRU_PLL_SetSource(CRU_PLL_SRC_C0);

    /** Configure PLL */
    CRU_PLL_ForceReset();
    CRU_PLL_Config(1UL, 48UL, 6UL, 48UL);
    CRU_PLL_ReleaseReset();

    /** PLL lock time min. 500 cycles */
    __delay_cycles(500UL);

    /** Configure CCLK */
    CRU_SetCCLKPrescaler(CRU_CLK_DIV_1);
    CRU_CCLKPrescaler_Enable();
    CRU_SetCCLKSource(CRU_CLK_SRC_PLL);

    /** Configure PCLK0 */
    CRU_SetPCLK0Prescaler(CRU_CLK_DIV_2);
    CRU_PCLK0Prescaler_Enable();
    CRU_SetPCLK0Source(CRU_CLK_SRC_PLL);

    /** Configure PCLK1 */
    CRU_SetPCLK1Prescaler(CRU_CLK_DIV_2);
    CRU_PCLK1Prescaler_Enable();
    CRU_SetPCLK1Source(CRU_CLK_SRC_PLL);

    /** Configure PCLK2 */
    CRU_SetPCLK2Prescaler(CRU_CLK_DIV_2);
    CRU_PCLK2Prescaler_Enable();
    CRU_SetPCLK2Source(CRU_CLK_SRC_PLL);

    /** Configure HCLK */
    CRU_SetHCLKPrescaler(CRU_CLK_DIV_2);
    CRU_HCLKPrescaler_Enable();
    CRU_SetHCLKSource(CRU_CLK_SRC_PLL);

    /** Configure 1MHz clocks */
    CRU_SetCCLK1MHzPrescaler(199UL);
    CRU_SetHCLK1MHzPrescaler(99UL);
}


int
main (void) {
    /* Prepare eFlash to 200 MHz CCLK. See PLL configuration */
    EFLASH_Init(200000000UL);
    CLK_Init();
    bsp_serial_init();
    setbuf(stdout, NULL);

    CLIC_Config(1U, 1U);
    CLIC_SetLevelThreshold(0U);
    __enable_irq();

    printf("DFU example\r\n\tbuild: %s %s\r\n", __DATE__, __TIME__);
    printf("\t\"help\" for more info\r\n");

    for (;;) {
        dfu_init();
        dfu_exec();
    }

	return 0;
}
