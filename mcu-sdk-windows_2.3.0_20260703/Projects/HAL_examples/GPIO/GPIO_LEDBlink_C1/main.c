/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/GPIO/GPIO_LEDBlink_C1/main.c
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
#include "bmcu_common.h"
#include "bmcu_cru.h"

/** @brief  Function to be executed by the Core 1 */
void
core1_code (void) {
    for (;;) {
        // The LED GPIO is preconfigured by the Core 0
        bsp_led_toggle();
        __delay_ms(500UL);
    }
}

/** @brief  Function to be executed by the Core 0 */
int
main (void) {
    //Initialize the LED GPIO pin as output
    bsp_led_init();
    
    // Reset the Core 1
    CRU_C1_ForceResetAll();
    
    // Start the Core 1 code execution from the desired address
    CRU_C1_Start((uint32_t)core1_code);

    // infinit loop for the Core 0
    while (1);
	return 0;
}

