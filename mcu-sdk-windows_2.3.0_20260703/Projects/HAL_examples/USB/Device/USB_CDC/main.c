/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/USB/Device/USB_CDC/main.c
 *  @author     Baikal electronics SDK team
 *  @brief      USB CDC class example
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

/**
 * @brief Clock configuration:
 *
 * PLL   = 200MHz
 * CCLK  = 100MHz
 * PCLK0 = 100MHz
 * PCLK1 = 50MHz
 * PCLK2 = 100MHz
 * HCLK  = 50MHz
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

    CRU_SetPCLK1Prescaler(CRU_CLK_DIV_4);
    CRU_PCLK1Prescaler_Enable();
    CRU_SetPCLK1Source(CRU_CLK_SRC_PLL);

    CRU_SetPCLK2Prescaler(CRU_CLK_DIV_2);
    CRU_PCLK2Prescaler_Enable();
    CRU_SetPCLK2Source(CRU_CLK_SRC_PLL);

    CRU_SetHCLKPrescaler(CRU_CLK_DIV_4);
    CRU_HCLKPrescaler_Enable();
    CRU_SetHCLKSource(CRU_CLK_SRC_PLL);

    /** Configure 1MHz clocks */
    CRU_SetCCLK1MHzPrescaler(99UL);
    CRU_SetHCLK1MHzPrescaler(49UL);

    CRU_USB_PHY_PLL_Enable();
    __delay_cycles(100UL);
    CRU_USB_PHY_ReleaseReset();
    __delay_us(500UL);
}

void
init (void) {
    /* Prepare eFlash to 100 MHz CCLK. See PLL configuration */
    EFLASH_Init(100000000UL);

    CLK_Init();

    CLIC_Config(1U, 1U);
    CLIC_SetLevelThreshold(0U);

    bsp_serial_init();
    setbuf(stdout, NULL);           ///< Disable buffered stdout

    tusb_rhport_init_t dev_init = {
        .role = TUSB_ROLE_DEVICE,
        .speed = TUSB_SPEED_AUTO
    };
    tusb_init(BOARD_TUD_RHPORT, &dev_init);     ///< Init USB device

    __enable_irq();                 ///< Enable interrupts globally
}

void
cdc_task (void) {
    /// Connected and there are data available
    if (tud_cdc_available()) {
        /// Read data
        char buf[64];
        uint32_t count = tud_cdc_read(buf, sizeof(buf));

        // Echo back
        // Note: Skip echo by commenting out write() and write_flush()
        // for throughput test e.g
        //    $ dd if=/dev/zero of=/dev/ttyACM0 count=10000
        tud_cdc_write(buf, count);
        tud_cdc_write_flush();
    }
}

int
main (void) {
    init();
    printf("CDC example, %s %s\r\n", __DATE__, __TIME__);

    for (;;) {
        tud_task();
        cdc_task();
    }

	return 0;
}
