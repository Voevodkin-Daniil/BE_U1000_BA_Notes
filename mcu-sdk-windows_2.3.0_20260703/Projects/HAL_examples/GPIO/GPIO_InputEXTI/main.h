/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/GPIO/GPIO_InputEXTI/main.h
 *  @author     Baikal electronics SDK team
 *  @brief      GPIO input EXTI interrupt mode example header file
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
#ifndef __MAIN_H
#define __MAIN_H

#if defined EVU_BA_2_1 || defined EVU_BA_2_3 || defined EVU_BA_2_5
#define LED_GPIO_PORT  (GPIO2)
#define LED_GPIO_PIN   (GPIO_PIN_0)
#define LED_CRU_PORT   (CRU_PORT_C)
#define LED_CRU_PIN    (CRU_PIN_0)
#define LED_CLK_EN_FN  (CRU_APB2_EnableClock)
#define LED_CLK_PERIPH (CRU_APB2_PERIPH_GPIO2)
#define BTN_GPIO_PORT  (GPIO2)
#define BTN_GPIO_PIN   (GPIO_PIN_13)
#define BTN_CRU_PORT   (CRU_PORT_C)
#define BTN_CRU_PIN    (CRU_PIN_13)
#define BTN_CLK_EN_FN  (CRU_APB2_EnableClock)
#define BTN_CLK_PERIPH (CRU_APB2_PERIPH_GPIO2)
#define BTN_CLIC_IRQN  (CLIC_EXTI_PC_IRQn)
#define BTN_ISR        (EXTI_PC_IRQHandler)
#elif defined EVU_BA_2_0 || defined EVU_BA_1_2  // The same LED and user button pins
#define LED_GPIO_PORT  (GPIO1)
#define LED_GPIO_PIN   (GPIO_PIN_10)
#define LED_CRU_PORT   (CRU_PORT_B)
#define LED_CRU_PIN    (CRU_PIN_10)
#define LED_CLK_EN_FN  (CRU_APB1_EnableClock)
#define LED_CLK_PERIPH (CRU_APB1_PERIPH_GPIO1)
#define BTN_GPIO_PORT  (GPIO2)
#define BTN_GPIO_PIN   (GPIO_PIN_13)
#define BTN_CRU_PORT   (CRU_PORT_C)
#define BTN_CRU_PIN    (CRU_PIN_13)
#define BTN_CLK_EN_FN  (CRU_APB2_EnableClock)
#define BTN_CLK_PERIPH (CRU_APB2_PERIPH_GPIO2)
#define BTN_CLIC_IRQN  (CLIC_EXTI_PC_IRQn)
#define BTN_ISR        (EXTI_PC_IRQHandler)
#else
// Add definitions for the custom board
// #define LED_GPIO_PORT
// #define LED_GPIO_PIN
// #define LED_CRU_PORT
// #define LED_CRU_PIN
// #define LED_CLK_EN_FN
// #define LED_CLK_PERIPH
// #define BTN_GPIO_PORT
// #define BTN_GPIO_PIN
// #define BTN_CRU_PORT
// #define BTN_CRU_PIN
// #define BTN_CLK_EN_FN
// #define BTN_CLK_PERIPH
// #define BTN_CLIC_IRQN
// #define BTN_ISR
#endif

#if !defined(LED_GPIO_PORT) || !defined(LED_GPIO_PIN)   || \
    !defined(LED_CRU_PORT)  || !defined(LED_CRU_PIN)    || \
    !defined(LED_CLK_EN_FN) || !defined(LED_CLK_PERIPH) || \
    !defined(BTN_GPIO_PORT) || !defined(BTN_GPIO_PIN)   || \
    !defined(BTN_CRU_PORT)  || !defined(BTN_CRU_PIN)    || \
    !defined(BTN_CLK_EN_FN) || !defined(BTN_CLK_PERIPH) || \
    !defined(BTN_CLIC_IRQN) || !defined(BTN_ISR)
#error "The required definitions are missing"
#endif
#endif /* __MAIN_H */
