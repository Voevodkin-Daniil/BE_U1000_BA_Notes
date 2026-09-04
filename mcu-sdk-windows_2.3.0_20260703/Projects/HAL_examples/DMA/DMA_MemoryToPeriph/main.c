/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/DMA/DMA_MemoryToPeriph/main.c
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
#include <string.h>

#include "bmcu_common.h"
#include "bmcu_cru.h"
#include "bmcu_dma.h"
#include "bmcu_uart.h"

#include "bsp.h"

#define TX_BUFFER_LENGTH 128

char TxBuffer[TX_BUFFER_LENGTH] __ALIGNED(4);
const char TestStr[] = "DMA Memory to Peripheral Transfer Example\r\n";

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

    /* Set DMA mode */
    UART_DMA_SetMode(BSP_UART_INSTANCE, UART_DMA_MODE0);
}

void DMA0_Channel0_Init(void)
{
    /* Deinit DMA channel */
    DMA_DeInit(DMA0, DMA_CH0);

    /* Configure DMA transfer */
    DMA_InitStruct_TypeDef DMA_InitStruct;
    DMA_StructInit(&DMA_InitStruct);

    DMA_InitStruct.SrcAddress = (uint32_t)TxBuffer;
    DMA_InitStruct.SrcMode = DMA_MODE_NORMAL;
    DMA_InitStruct.SrcIncMode = DMA_INC_MODE_INCREMENT;
    DMA_InitStruct.SrcTransferWidth = DMA_TR_WIDTH_8BITS;
    DMA_InitStruct.SrcBurstLength = DMA_BURST_LENGTH_4;
    DMA_InitStruct.SrcLinkedListCtrl = DISABLE;
    DMA_InitStruct.SrcHandshake = DMA_HS_HARDWARE;
    DMA_InitStruct.SrcHWHandshakePolarity = DMA_HS_POLARITY_HIGH;
    DMA_InitStruct.SrcHWHandshakeInterface = 0U;

    DMA_InitStruct.DstAddress = UART_DMA_GetRegAddr(BSP_UART_INSTANCE);
    DMA_InitStruct.DstMode = DMA_MODE_NORMAL;
    DMA_InitStruct.DstIncMode = DMA_INC_MODE_NO_CHANGE;
    DMA_InitStruct.DstTransferWidth = DMA_TR_WIDTH_8BITS;
    DMA_InitStruct.DstBurstLength = DMA_BURST_LENGTH_4;
    DMA_InitStruct.DstLinkedListCtrl = DISABLE;
    DMA_InitStruct.DstHandshake = DMA_HS_HARDWARE;
    DMA_InitStruct.DstHWHandshakePolarity = DMA_HS_POLARITY_HIGH;
    DMA_InitStruct.DstHWHandshakeInterface = 8U;

    DMA_InitStruct.Direction = DMA_DIR_MEMORY_TO_PERIPH;
    DMA_InitStruct.Priority = DMA_PRIORITY_7;
    DMA_InitStruct.FIFOMode = DMA_FIFO_MODE_0;
    DMA_InitStruct.LinkedListPointer = 0x0UL;
    DMA_InitStruct.NbData = sizeof(TxBuffer);

    /* Init DMA channel */
    DMA_Init(DMA0, DMA_CH0, &DMA_InitStruct);

    /* Disable DMA channel interrupts */
    DMA_DisableIT(DMA0, DMA_CH0);

    /* Enable DMA */
    DMA_Enable(DMA0);

    /* Clear any pending interrupts */
    DMA_ClearIT_IntTfr(DMA0, DMA_CH0);
    DMA_ClearIT_IntBlock(DMA0, DMA_CH0);
    DMA_ClearIT_IntSrcTran(DMA0, DMA_CH0);
    DMA_ClearIT_IntDstTran(DMA0, DMA_CH0);
    DMA_ClearIT_IntErr(DMA0, DMA_CH0);
}

void Init(void)
{
    /* Prepare eFlash to 200 MHz CCLK. See PLL configuration */
    EFLASH_Init(200000000UL);
    
    /* Init clock */
    CLK_Init();

    /* Init UART */
    BSP_UART_Init();
}

int main(void)
{
    Init();

    for(;;)
    {
        memset(TxBuffer, '\0', TX_BUFFER_LENGTH);
        strcpy(TxBuffer, TestStr);

        /* Init DMA channel and configure DMA transfer */
        DMA0_Channel0_Init();

        /* Enable DMA channel */
        DMA_EnableChannel(DMA0, DMA_CH0);

        /* Wait for DMA transfer to complete */
        while (DMA_IsActiveIT_IntTfr(DMA0, DMA_CH0) != 1UL);

        __delay_ms(1000UL);
    }

	return 0;
}
