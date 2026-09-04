/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/GPIO/GPIO_InputPolling/main.c
 *  @author     Baikal electronics SDK team
 *  @brief      GPIO input polling mode example source file
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
#include "main.h"
#include "bmcu_cru.h"
#include "bmcu_gpio.h"


static void
led_init (void) {
    CRU_PIN_InitStruct_TypeDef CRU_PIN_InitStruct;
    GPIO_InitStruct_TypeDef GPIO_InitStruct;

    /* Initialize CRU settings of the LED pin */
    CRU_PIN_StructInit(&CRU_PIN_InitStruct);
    CRU_PIN_InitStruct.Port          = LED_CRU_PORT;
    CRU_PIN_InitStruct.Pin           = LED_CRU_PIN;
    CRU_PIN_InitStruct.Pull          = CRU_PIN_PULL_NO;
    CRU_PIN_InitStruct.InputCtrl     = DISABLE;
    CRU_PIN_InitStruct.DriveStrength = CRU_PIN_DRIVE_STRENGTH_0;
    CRU_PIN_InitStruct.Alternate     = CRU_PIN_AF_0;
    CRU_PIN_Init(&CRU_PIN_InitStruct);

    /* Initialize GPIO settings of the LED pin */
    /* Enable clock of the LED GPIO port */
    LED_CLK_EN_FN(LED_CLK_PERIPH);

    /* Configure the LED pin as output */
    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.PinMask = LED_GPIO_PIN;
    GPIO_InitStruct.Mode    = GPIO_MODE_OUTPUT;
    GPIO_Init(LED_GPIO_PORT, &GPIO_InitStruct);

}


static void
btn_init (void) {
    CRU_PIN_InitStruct_TypeDef CRU_PIN_InitStruct;
    GPIO_InitStruct_TypeDef GPIO_InitStruct;

    /* Initialize CRU settings of the user button pin */
    CRU_PIN_StructInit(&CRU_PIN_InitStruct);
    CRU_PIN_InitStruct.Port          = BTN_CRU_PORT;
    CRU_PIN_InitStruct.Pin           = BTN_CRU_PIN;
    CRU_PIN_InitStruct.Pull          = CRU_PIN_PULL_UP;
    CRU_PIN_InitStruct.InputCtrl     = ENABLE;
    CRU_PIN_InitStruct.DriveStrength = CRU_PIN_DRIVE_STRENGTH_0;
    CRU_PIN_InitStruct.Alternate     = CRU_PIN_AF_0;
    CRU_PIN_Init(&CRU_PIN_InitStruct);

    /* Initialize GPIO settings of the user button pin */
    /* Enable clock of the user button GPIO port */
    BTN_CLK_EN_FN(BTN_CLK_PERIPH);

    /* Configure the user button pin as input */
    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.PinMask = BTN_GPIO_PIN;
    GPIO_InitStruct.Mode    = GPIO_MODE_INPUT;
    GPIO_Init(BTN_GPIO_PORT, &GPIO_InitStruct);
}

int
main (void) {
    uint8_t btn_state = 0;
    uint8_t btn_prev_state = 0;

    led_init();
    btn_init();

    btn_state = GPIO_IsInputPinSet(BTN_GPIO_PORT, BTN_GPIO_PIN);
    btn_prev_state = btn_state;

    /* User button hardware debounce supposed */
    for (;;) {
        btn_state = GPIO_IsInputPinSet(BTN_GPIO_PORT, BTN_GPIO_PIN);

        if (btn_state == 0 && btn_prev_state == 1) {
            GPIO_ToggleOutputPin(LED_GPIO_PORT, LED_GPIO_PIN);
        }

        btn_prev_state = btn_state;
    }

	return 0;
}
