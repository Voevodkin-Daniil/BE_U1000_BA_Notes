/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/Benchmarks/USB/Device/USB_CDC_TestBW_RX/main.c
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

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "bmcu_common.h"
#include "bmcu_cru.h"
#include "tusb.h"
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
 * Clock configuration:
 * PLL   = 200MHz
 * CCLK  = 200MHz
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

    CRU_SetCCLKPrescaler(CRU_CLK_DIV_1);
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

    CRU_USB_PHY_PLL_Enable();
    __delay_cycles(100UL);
    CRU_USB_PHY_ReleaseReset();
    __delay_us(500UL);
}

void
init (void) {
    /* Prepare eFlash to 200 MHz CCLK. See PLL configuration */
    EFLASH_Init(200000000UL);

    CLK_Init();

    CLIC_Config(1U, 1U);
    CLIC_SetLevelThreshold(0U);

    bsp_serial_init();

    /* Disable buffered stdout */
    setbuf(stdout, NULL);

    /* Init USB device */
    tusb_rhport_init_t dev_init = {
        .role = TUSB_ROLE_DEVICE,
        .speed = TUSB_SPEED_AUTO
    };
    tusb_init(BOARD_TUD_RHPORT, &dev_init);  

    /* Enable interrupts globally */
    __enable_irq();
}

int
main (void) {
    init();
    printf("USB benchmark, %s %s\r\n", __DATE__, __TIME__);

    for (;;) {
        tud_task();
    }

	return 0;
}
