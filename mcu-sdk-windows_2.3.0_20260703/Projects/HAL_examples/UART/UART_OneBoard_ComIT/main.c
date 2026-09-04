/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/UART/UART_OneBoard_ComIT/main.c
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
#include <string.h>
#include "bmcu_cru.h"
#include "bmcu_uart.h"
#include "bsp.h"

#define BUF_LEN (128U)

uint8_t rx_buf[BUF_LEN];
uint8_t tx_buf[BUF_LEN];

void
uart_Init (void) {
    bsp_serial_init();

    /* Enable Receive Data Available interrupt */
    UART_EnableIT(BSP_UART_INSTANCE, UART_IT_RDA);

    /* Configure UART interrupt in CLIC */
    CLIC_ConfigIRQ(BSP_UART_CLIC_IRQN,              /* Interrupt */
                   CLIC_INTATTR_MODE_MACHINE,       /* Privilege mode */
                   1U,                              /* Level */
                   1U,                              /* Priority */
                   CLIC_INTATTR_SHV_VECTORED,       /* Vector mode */
                   CLIC_INTATTR_TRIG_TYPE_LEVEL,    /* Type */
                   CLIC_INTATTR_TRIG_POL_P);        /* Polarity */
    CLIC_EnableIRQ(BSP_UART_CLIC_IRQN);
}


int
main (void) {
    uart_Init();

    /* Configure CLIC and enable interrupts globally */
    CLIC_Config(1U, 1U);
    CLIC_SetLevelThreshold(0U);
    __enable_irq();

    while (1);
	return 0;
}


void __attribute__ ((interrupt))
BSP_UART_ISR (void) {
    static uint32_t rx_ch_idx;
    static uint32_t tx_ch_idx;
    uint32_t status = 0;
    uint8_t ch = 0;

    UART_IID_TypeDef irq_id = UART_GetIID(BSP_UART_INSTANCE);

    /* Received data available */
    if (UART_IID_RX_DATA_AVAILABLE == irq_id) {
        /* Get line status */
        status = UART_GetLineStatus(BSP_UART_INSTANCE);

        /* Check if data received */
        if (status & UART_LINE_STATUS_DR) {
            ch = UART_ReceiveData8b(BSP_UART_INSTANCE);
            
            /* If no UART line errors occured */
            if (!(status & UART_LINE_STATUS_OE) &&
                !(status & UART_LINE_STATUS_PE) &&
                !(status & UART_LINE_STATUS_FE) &&
                !(status & UART_LINE_STATUS_BI)) {
                rx_buf[rx_ch_idx] = ch;

                /* Rx index: increment  and check for overflow */
                rx_ch_idx = (rx_ch_idx + 1) % BUF_LEN;

                /* Send the line back if newline character received
                and Tx buffer is not being sent */
                if (('\r' == ch || '\n' == ch) && 0 == tx_ch_idx) {
                    /* Copy line to Tx buffer */
                    memcpy(tx_buf, rx_buf, BUF_LEN);

                    /* Start a new line to improve readability of the output */
                    bsp_serial_putchar('\r');
                    bsp_serial_putchar('\n');

                    /* Enable Transmit Holding Register Empty interrupt */
                    UART_EnableIT(BSP_UART_INSTANCE, UART_IT_THRE);

                    rx_ch_idx = 0;
                }
            }
        }
    
    /* Transmitter Holding Register empty */
    } else if (UART_IID_THR_EMPTY == irq_id) {
        /* Send character */
        ch = tx_buf[tx_ch_idx++];
        UART_TransmitData8b(BSP_UART_INSTANCE, ch);

        /* Check for the end of line */
        if (('\r' == ch) || ('\n' == ch) || (BUF_LEN == tx_ch_idx)) {
            /* Reset Tx buffer pointer to indicate that there is no data being sent */
            tx_ch_idx = 0;

            /* Disable Transmit Holding Register Empty interrupt */
            UART_DisableIT(BSP_UART_INSTANCE, UART_IT_THRE);
        }
    }
}
