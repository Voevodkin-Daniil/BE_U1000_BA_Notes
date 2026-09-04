/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/USB/Device/USB_HID_Keyboard/main.c
 *  @author     Baikal electronics SDK team
 *  @brief      USB HID class example
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
    bsp_btn_init();

    bsp_serial_init();
    setbuf(stdout, NULL);           /* Disable buffered stdout */

    tusb_rhport_init_t dev_init = {
        .role = TUSB_ROLE_DEVICE,
        .speed = TUSB_SPEED_AUTO
    };
    tusb_init(BOARD_TUD_RHPORT, &dev_init);     /* Init USB device */

    __enable_irq();                 /* Enable interrupts globally */
}

/* Invoked when received GET_REPORT control request.
   Application must fill buffer report's content and return its length.
   Return zero will cause the stack to STALL request. */
uint16_t
tud_hid_get_report_cb (uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen) {
    (void)itf;
    (void)report_id;
    (void)report_type;
    (void)buffer;
    (void)reqlen;

    return 0U;
}

/* Invoked when received SET_REPORT control request or 
   received data on OUT endpoint ( Report ID = 0, Type = 0 ) */
void
tud_hid_set_report_cb (uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize) {
    (void)itf;
    (void)report_id;
    (void)report_type;
    (void)buffer;
    (void)bufsize;

    /* Keyboard output report format: */
    /* Bit[0]    - NUM LOCK           */
    /* Bit[1]    - SCROLL LOCK        */
    /* Bit[2]    - CAPS LOCK          */
    /* Bit[3..7] - Reserved           */
    if ((report_id == 0x0U) &&
        (report_type == 0x2U))
    {
        /* Output report */
        if (buffer[0] & 0x2U)
        {
            bsp_led_on();
        }
        else
        {
            bsp_led_off();
        }
    }
}

void
hid_task (void) {
    static uint64_t prev_time_us = 0ULL;
    
    /* Poll user button every 10ms */
    uint64_t curr_time_us = __get_time();
    if ((curr_time_us - prev_time_us) > 10000UL)
    {
        uint8_t btn_state = bsp_btn_ispressed();

        /* Remote wakeup */
        if (tud_suspended() && (btn_state != 0UL))
        {
            /* Wake up host if we are in suspend mode
               and REMOTE_WAKEUP feature is enabled by host */
            tud_remote_wakeup();
        }

        /* Keyboard device */
        if (tud_hid_ready())
        {
            static uint8_t btn_prev_state = 0UL;

            /* Keyboard input report format: */
            /* Byte[0] - Modifier Keys       */
            /* Byte[1] - Reserved            */
            /* Byte[2] - Keycode 1           */
            /* Byte[3] - Keycode 2           */
            /* Byte[4] - Keycode 3           */
            /* Byte[5] - Keycode 4           */
            /* Byte[6] - Keycode 5           */
            /* Byte[7] - Keycode 6           */

            uint8_t keycode[6] = { 0 };
            if (btn_state != 0UL)
            {
                /* User button is pressed */
                keycode[0] = HID_KEY_CAPS_LOCK;

                tud_hid_keyboard_report(0, 0, keycode);
            }
            else
            {
                /* User button is resealed */

                /* Send empty key report only once on release */
                if (btn_prev_state != 0UL)
                {
                    tud_hid_keyboard_report(0, 0, HID_KEY_NONE);
                }
            }

            btn_prev_state = btn_state;
        }

        prev_time_us = curr_time_us;
    }
}

int
main(void) {
    init();
    printf("HID example, %s %s\r\n", __DATE__, __TIME__);

    for (;;)
    {
        tud_task();
        hid_task();
    }
}
