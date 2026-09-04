/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/QSPI/QSPI_Flash/main.c
 *  @author     Baikal electronics SDK team
 *  @brief      QSPI example
 *  @version    2.3.0
 *  @date       20260703
 *
 *  File content:
 *      - Periphery configuration functions (UART, LED, Timer)
 *      - The main loop function
 * *****************************************************************************
 *  @copyright Copyright (c) 2025 Baikal Electronics JSC
 *
 *  This software is licensed under terms that can be found in the LICENSE file
 *  in the root directory of this software component.
 *  If no LICENSE file comes with this software, it is provided AS-IS.
 * *****************************************************************************
 */

#include "bmcu_cru.h"
#include "bmcu_uart.h"
#include "bmcu_gpio.h"
#include "cli.h"

extern volatile uint8_t cli_rx_char;    ///< New character code from the UART ISR

/**
 * @brief   UART0 char output function
 * @param   ch code of received UART character
 * @retval  the code of received UART character
 */
int
__io_putchar (int ch) {
    while ((UART_GetLineStatus(UART0) & UART_LSR_THRE) == 0UL);
    UART_TransmitData8b(UART0, (uint8_t)ch);

    return ch;
}

/**
 * @brief   CPU clock setup (PLL, Core, APB[0..2], AHB)
 */
void
CLK_Init (void) {
    CRU_PLL_BypassMode_Disable();
    
    /// Set PLL source clock to C0
    CRU_PLL_SetSource(CRU_PLL_SRC_C0);

    /// PLL: 200 MHz
    CRU_PLL_ForceReset();
    CRU_PLL_Config(1UL, 48UL, 6UL, 48UL);
    CRU_PLL_ReleaseReset();

    /// PLL lock time min. 500 cycles
    __delay_cycles(500UL);

    /// CCLK: 200 MHz
    CRU_SetCCLKPrescaler(CRU_CLK_DIV_1);
    CRU_CCLKPrescaler_Enable();
    CRU_SetCCLKSource(CRU_CLK_SRC_PLL);

    /// PCLK0: 100 MHz
    CRU_SetPCLK0Prescaler(CRU_CLK_DIV_2);
    CRU_PCLK0Prescaler_Enable();
    CRU_SetPCLK0Source(CRU_CLK_SRC_PLL);

    /// PCLK1: 100 MHz
    CRU_SetPCLK1Prescaler(CRU_CLK_DIV_2);
    CRU_PCLK1Prescaler_Enable();
    CRU_SetPCLK1Source(CRU_CLK_SRC_PLL);

    /// PCLK2: 100 MHz
    CRU_SetPCLK2Prescaler(CRU_CLK_DIV_2);
    CRU_PCLK2Prescaler_Enable();
    CRU_SetPCLK2Source(CRU_CLK_SRC_PLL);

    /// HCLK: 100 MHz
    CRU_SetHCLKPrescaler(CRU_CLK_DIV_2);
    CRU_HCLKPrescaler_Enable();
    CRU_SetHCLKSource(CRU_CLK_SRC_PLL);

    /// 1MHz clocks
    CRU_SetCCLK1MHzPrescaler(199UL);
    CRU_SetHCLK1MHzPrescaler(99UL);
}

/**
 * @brief   UART0 and interrupt configuration
 */
void
UART0_Init (void) {
    CRU_APB0_EnableClock(CRU_APB0_PERIPH_UART0);
    CRU_APB0_EnableClock(CRU_APB0_PERIPH_GPIO0);
    CRU_SetPinAF(CRU_PORT_A, CRU_PIN_6, CRU_PIN_AF_1);
    CRU_SetPinAF(CRU_PORT_A, CRU_PIN_7, CRU_PIN_AF_1);
    UART_InitStruct_TypeDef UART_InitStruct = {
        .BaudRate   = 115200UL,
        .DataWidth  = UART_DATAWIDTH_8B,
        .StopBits   = UART_STOP_1BIT,
        .Parity     = UART_PARITY_NONE,
        .Transfer9b = UART_TRANSFER_9B_DISABLE,
        .CtrlFIFO   = DISABLE
    };

    UART_Init(UART0, &UART_InitStruct);

    UART_EnableIT(UART0, UART_IT_RDA);

    CLIC_SetLevel(CLIC_UART0_IRQn, 1);

    CLIC_SetVectorMode(CLIC_UART0_IRQn, CLIC_INTATTR_SHV_VECTORED);
    CLIC_SetTrigType(CLIC_UART0_IRQn, CLIC_INTATTR_TRIG_TYPE_LEVEL);
    CLIC_SetTrigPolarity(CLIC_UART0_IRQn, CLIC_INTATTR_TRIG_POL_P);
    CLIC_SetPrivMode(CLIC_UART0_IRQn, CLIC_INTATTR_MODE_MACHINE);

    CLIC_EnableIRQ(CLIC_UART0_IRQn);
}

int
main (void) {
    /* Prepare eFlash to 200 MHz CCLK. See PLL configuration */
    EFLASH_Init(200000000);
    
    CLK_Init();
    UART0_Init();
    cli_init();

    CLIC_Config(1U, 1U);
    CLIC_SetLevelThreshold(0U);
    __enable_irq();

    for (;;) {
        if (cli_rx_char) {
            cli_char_rx_callback(cli_rx_char);
        }
    }

    return 0;
}
