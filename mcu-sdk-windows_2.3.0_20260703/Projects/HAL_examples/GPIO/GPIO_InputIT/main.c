/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/GPIO/GPIO_InputIT/main.c
 *  @author     Baikal electronics SDK team
 *  @brief      GPIO input interrupt mode example source file
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

    /* Configure interrupt for the user button GPIO pin */
    GPIO_SetPinITType(BTN_GPIO_PORT, BTN_GPIO_PIN, BTN_IT_TYPE);
    GPIO_SetPinBothEdgeIT(BTN_GPIO_PORT, BTN_GPIO_PIN, BTN_IT_MODE);
    GPIO_SetPinITPolarity(BTN_GPIO_PORT, BTN_GPIO_PIN, BTN_IT_POL);
    GPIO_SetPinDebounce(BTN_GPIO_PORT, BTN_GPIO_PIN, GPIO_IT_DEBOUNCE_ENABLE);
    GPIO_SetPinIT(BTN_GPIO_PORT, BTN_GPIO_PIN, GPIO_IT_ENABLE);

    /* Configure CLIC for the GPIO interrupt */
    CLIC_ConfigIRQ(BTN_CLIC_IRQN,                   /* Interrupt */
                   CLIC_INTATTR_MODE_MACHINE,       /* Privilege mode */
                   1U,                              /* Level */
                   1U,                              /* Priority */
                   CLIC_INTATTR_SHV_VECTORED,       /* Vector mode */
                   CLIC_INTATTR_TRIG_TYPE_LEVEL,    /* Type */
                   CLIC_INTATTR_TRIG_POL_P);        /* Polarity */

    /* Enable GPIO interrupt */
    CLIC_EnableIRQ(BTN_CLIC_IRQN);
}


int
main (void) {
    led_init();
    btn_init();

    /* Init CLIC */
    CLIC_Config(1U, 1U);
    CLIC_SetLevelThreshold(0U);

    /* Enable interrupts globally */
    __enable_irq();

    for (;;) {
        // Nothing to do
    };

	return 0;
}


/* GPIO port ISR
There is no all GPIO pins interrupt status check because only one GPIO pin
is configured as an interrupt source */
__attribute__ ((interrupt)) void
BTN_ISR (void) {
    GPIO_ClearPinIT(BTN_GPIO_PORT, BTN_GPIO_PIN);
    GPIO_ToggleOutputPin(LED_GPIO_PORT, LED_GPIO_PIN);
}
