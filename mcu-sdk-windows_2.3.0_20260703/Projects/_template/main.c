/**
 * *****************************************************************************
 *  @file       Projects/_template/main.c
 *  @author     Baikal electronics SDK team
 *  @brief      Template example source file
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
#include "bmcu_cru.h"
#include "bmcu_gpio.h"
#include "module1.h"
#include "module2.h"

#define LED_GPIO GPIO1
#define LED_PIN  GPIO_PIN_10

int glob_uninit_var;
int glob_init_var = 2600;

int
main (void) {
    static int static_loc_var;
    int loc_uninit_var;
    int loc_init_var = 42;

    loc_uninit_var = (module1_func() + module2_2_func()) + \
                      glob_uninit_var + glob_init_var +    \
                      static_loc_var + loc_init_var;

    /* Enable GPIO1 clock */
    CRU_APB1_EnableClock(CRU_APB1_PERIPH_GPIO1);

    /* Configure LED pin as output */
    GPIO_SetPinMode(LED_GPIO, LED_PIN, GPIO_MODE_OUTPUT);

    while (1) {
        GPIO_ToggleOutputPin(LED_GPIO, LED_PIN);
        __delay_ms(500UL);
    }

    return 0;
}
