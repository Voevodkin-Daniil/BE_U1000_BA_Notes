/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/USB/Device/USB_MSC_Flash/main.c
 *  @author     Baikal electronics SDK team
 *  @brief      USB MSC class example
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
#include "bmcu_eflash.h"
#include "bmcu_qspi.h"
#include "tusb.h"
#include "flash_mt25ql128aba.h"
#include "bsp.h"

/* Device states */
enum {
    STATE_NOT_MOUNTED = 0,
    STATE_MOUNTED,
    STATE_SUSPENDED
};

int
__io_putchar (int ch) {
    return bsp_serial_putchar(ch);
}

static int32_t device_state;

static void led_task(void);

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

    CRU_SetCCLK1MHzPrescaler(99UL);
    CRU_SetHCLK1MHzPrescaler(99UL);

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

    bsp_led_init();

    bsp_serial_init();
    setbuf(stdout, NULL);           /* Disable buffered stdout */

    FLASH_Init();
    FLASH_EnterQuadMode();

    device_state = STATE_NOT_MOUNTED;

    tusb_rhport_init_t dev_init = {
        .role = TUSB_ROLE_DEVICE,
        .speed = TUSB_SPEED_AUTO
    };
    tusb_init(BOARD_TUD_RHPORT, &dev_init);     /* Init USB device */

    __enable_irq();                 /* Enable interrupts globally */
}

/* Invoked when device is mounted */
void
tud_mount_cb(void) {
    device_state = STATE_MOUNTED;
}

/* Invoked when device is unmounted */
void
tud_umount_cb(void) {
    device_state = STATE_NOT_MOUNTED;

    msc_sync();
}

/* Invoked when usb bus is suspended */
void
tud_suspend_cb(bool remote_wakeup_en) {
    (void)remote_wakeup_en;
    device_state = STATE_SUSPENDED;

    /* Windows OS suspends a USB device rather than unmounts it.
       For this reason sync storage on suspend as well. */
    msc_sync();
}

/* Invoked when usb bus is resumed */
void
tud_resume_cb(void) {
    device_state = tud_mounted() ? STATE_MOUNTED : STATE_NOT_MOUNTED;
}

void
led_task(void) {
    switch(device_state)
    {
        case STATE_MOUNTED:
        {
            bsp_led_on();
        }
        break;

        case STATE_SUSPENDED:
        {
            static uint64_t prev_time_us = 0ULL;
            uint64_t curr_time_us = __get_time();
            if ((curr_time_us - prev_time_us) > 2000000UL)
            {
                bsp_led_toggle();
                prev_time_us = curr_time_us;
            }
        }
        break;

        case STATE_NOT_MOUNTED:
        default:
        {
            bsp_led_off();
        }
        break;
    }
}

int
main(void) {
    init();
    printf("MSC example, %s %s\r\n", __DATE__, __TIME__);

    msc_init();

    uint64_t curr_time_us = __get_time();
    msc_set_write_time(curr_time_us);

    for (;;) {
        tud_task();
        led_task();

        /* Sync on timer */
        curr_time_us = __get_time();
        if ((msc_get_modified_flag() != 0U) &&
            ((curr_time_us - msc_get_write_time()) > 5000000UL))
        {
            msc_sync();
            msc_set_write_time(curr_time_us);
        }
    }

    return 0;
}
