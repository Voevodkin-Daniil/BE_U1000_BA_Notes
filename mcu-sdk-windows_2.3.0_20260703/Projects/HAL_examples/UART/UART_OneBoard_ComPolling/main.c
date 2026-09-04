/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/UART/UART_OneBoard_ComPolling/main.c
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
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "bmcu_common.h"
#include "bmcu_cru.h"
#include "bmcu_uart.h"

#include "bsp.h"

#define UART_BUFFER_LENGTH (128U)

uint8_t UART_RxBuffer[UART_BUFFER_LENGTH];
uint8_t UART_TxBuffer[UART_BUFFER_LENGTH];

uint32_t UART_RxBuffer_Pointer;
uint32_t UART_TxBuffer_Pointer;

void CLK_Init(void)
{
    /*
     * Clock configuration:
     *
     * PLL = 200MHz
     * CCLK = 100MHz
     * PCLK0 = 100MHz
     * PCLK1 = 100MHz
     * PCLK2 = 100MHz
     * HCLK = 50MHz
     */

    CRU_PLL_BypassMode_Disable();

    /* Set PLL source clock to C0 */
    CRU_PLL_SetSource(CRU_PLL_SRC_C0);

    /* Configure PLL */
    CRU_PLL_ForceReset();
    CRU_PLL_Config(1UL, 48UL, 6UL, 48UL);
    CRU_PLL_ReleaseReset();

    /* PLL lock time min. 500 cycles */
    __delay_cycles(500UL);

    /* Configure CCLK */
    CRU_SetCCLKPrescaler(CRU_CLK_DIV_2);
    CRU_CCLKPrescaler_Enable();
    CRU_SetCCLKSource(CRU_CLK_SRC_PLL);

    /* Configure PCLK0 */
    CRU_SetPCLK0Prescaler(CRU_CLK_DIV_2);
    CRU_PCLK0Prescaler_Enable();
    CRU_SetPCLK0Source(CRU_CLK_SRC_PLL);

    /* Configure PCLK1 */
    CRU_SetPCLK1Prescaler(CRU_CLK_DIV_2);
    CRU_PCLK1Prescaler_Enable();
    CRU_SetPCLK1Source(CRU_CLK_SRC_PLL);

    /* Configure PCLK2 */
    CRU_SetPCLK2Prescaler(CRU_CLK_DIV_2);
    CRU_PCLK2Prescaler_Enable();
    CRU_SetPCLK2Source(CRU_CLK_SRC_PLL);

    /* Configure HCLK */
    CRU_SetHCLKPrescaler(CRU_CLK_DIV_4);
    CRU_HCLKPrescaler_Enable();
    CRU_SetHCLKSource(CRU_CLK_SRC_PLL);

    /* Configure 1MHz clocks */
    CRU_SetCCLK1MHzPrescaler(99UL);
    CRU_SetHCLK1MHzPrescaler(49UL);
}

void BSP_UART_Init(void)
{
#if defined (BMCU_U)
    /* Enable UART clock */
    BSP_UART_CLK_EN_FN(BSP_UART_CLK_PERIPH);

    /* Enable GPIO clock */
    BSP_UART_GPIO_CLK_EN_FN(BSP_UART_GPIO_CLK_PERIPH);
#endif

    /* Configure alternate function of UART pins */
    CRU_SetPinAF(BSP_UART_TX_CRU_PORT, BSP_UART_TX_CRU_PIN, BSP_UART_CRU_PIN_AF);
    CRU_SetPinAF(BSP_UART_RX_CRU_PORT, BSP_UART_RX_CRU_PIN, BSP_UART_CRU_PIN_AF);

    UART_DeInit(BSP_UART_INSTANCE);

    UART_InitStruct_TypeDef UART_InitStruct;
    UART_StructInit(&UART_InitStruct);

    UART_InitStruct.BaudRate = 115200UL;
    UART_InitStruct.DataWidth = UART_DATAWIDTH_8B;
    UART_InitStruct.StopBits = UART_STOP_1BIT;
    UART_InitStruct.Parity = UART_PARITY_NONE;
    UART_InitStruct.Transfer9b = UART_TRANSFER_9B_DISABLE;
    UART_InitStruct.CtrlFIFO = DISABLE;

    UART_Init(BSP_UART_INSTANCE, &UART_InitStruct);
}

void Init(void)
{
    /* Prepare eFlash to 100 MHz CCLK. See PLL configuration */
    EFLASH_Init(100000000);

    /* Init clock */
    CLK_Init();

    /* Init UART */
    BSP_UART_Init();
}

int main(void)
{
    UART_RxBuffer_Pointer = 0UL;
    UART_TxBuffer_Pointer = 0UL;

    Init();

    for(;;)
    {
        /* Get line status */
        uint32_t status = UART_GetLineStatus(BSP_UART_INSTANCE);

        /* Check if data received */
        if (status & UART_LINE_STATUS_DR)
        {
            uint8_t ch = UART_ReceiveData8b(BSP_UART_INSTANCE);

            if (!(status & UART_LINE_STATUS_OE) &&
                !(status & UART_LINE_STATUS_PE) &&
                !(status & UART_LINE_STATUS_FE) &&
                !(status & UART_LINE_STATUS_BI))
            {
                /* No error */

                UART_RxBuffer[UART_RxBuffer_Pointer++] = ch;

                /* Send the line back if newline character received */
                if (ch == '\n')
                {
                    /* Copy line to Tx buffer */
                    memcpy(UART_TxBuffer, UART_RxBuffer, UART_BUFFER_LENGTH);
                    /* Reset Tx buffer pointer */
                    UART_TxBuffer_Pointer = 0UL;

                    /* Send line back */
                    uint8_t ch;
                    do
                    {
                        /* Send character */
                        ch = UART_TxBuffer[UART_TxBuffer_Pointer++];

                        while ((UART_GetLineStatus(BSP_UART_INSTANCE) & UART_LINE_STATUS_THRE) == 0UL);
                        UART_TransmitData8b(BSP_UART_INSTANCE, ch);

                    } while ((ch != '\n') && (UART_TxBuffer_Pointer != UART_BUFFER_LENGTH));

                    UART_RxBuffer_Pointer = 0UL;
                }

                /* Check for overflow */
                if (UART_RxBuffer_Pointer >= UART_BUFFER_LENGTH)
                {
                    UART_RxBuffer_Pointer = 0UL;
                }
            }
        }
    }

	return 0;
}
