/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/GPIO/GPIO_InputPollingBSP/main.c
 *  @author     Baikal electronics SDK team
 *  @brief      GPIO input polling mode with BSP functions example source file
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
#include "bmcu_common.h"

int main(void)
{
    uint8_t btn_state = 0;
    uint8_t btn_prev_state = 0;
    uint8_t count = 0;

    bsp_led_init();
    bsp_btn_init();

    bsp_led_off();

    btn_state = bsp_btn_ispressed();
    btn_prev_state = btn_state;

    for (;;)
    {
        btn_state = bsp_btn_ispressed();

        /* Переход: кнопка была отпущена -> стала нажата */
        if (btn_state == 0 && btn_prev_state == 1)
        {
            count++;

            for (uint8_t i = 0; i < count; i++)
            {
                bsp_led_on();
                __delay_ms(100UL);

                bsp_led_off();
                __delay_ms(100UL);
            }
        }

        btn_prev_state = btn_state;
    }

    return 0;
}