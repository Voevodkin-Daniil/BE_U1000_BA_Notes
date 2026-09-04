/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/UART/UART_OneBoard_ComDMA/main.c
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
#include "bmcu_dma.h"

#include "bsp.h"

#define UART_BUFFER_LENGTH              (8U)

uint8_t UART_RxBuffer[UART_BUFFER_LENGTH];
uint8_t UART_TxBuffer[UART_BUFFER_LENGTH];

volatile uint8_t transmission_comlete = 0;

int __io_putchar(int ch)
{
    while ((UART_GetLineStatus(BSP_UART_INSTANCE) & UART_LINE_STATUS_THRE) == 0UL);
    UART_TransmitData8b(BSP_UART_INSTANCE, (uint8_t)ch);

    return ch;
}

int __io_getchar(void)
{
    return -1;
}

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

void DMA0_Channel2_Init(void)
{
    /* Deinit DMA channel */
    DMA_DeInit(DMA0, DMA_CH2);

    /* Configure DMA transfer */
    DMA_InitStruct_TypeDef DMA_InitStruct;
    DMA_StructInit(&DMA_InitStruct);

    DMA_InitStruct.SrcAddress = UART_DMA_GetRegAddr(BSP_UART_INSTANCE);
    DMA_InitStruct.SrcMode = DMA_MODE_NORMAL;
    DMA_InitStruct.SrcIncMode = DMA_INC_MODE_NO_CHANGE;
    DMA_InitStruct.SrcTransferWidth = DMA_TR_WIDTH_8BITS;
    DMA_InitStruct.SrcBurstLength = DMA_BURST_LENGTH_1;
    DMA_InitStruct.SrcLinkedListCtrl = DISABLE;
    DMA_InitStruct.SrcHandshake = DMA_HS_HARDWARE;
    DMA_InitStruct.SrcHWHandshakePolarity = DMA_HS_POLARITY_HIGH;
    DMA_InitStruct.SrcHWHandshakeInterface = 9U;

    DMA_InitStruct.DstAddress = (uint32_t)UART_RxBuffer;
    DMA_InitStruct.DstMode = DMA_MODE_AUTO_RELOAD;              // Only CH2 and CH3 can auto reload
    DMA_InitStruct.DstIncMode = DMA_INC_MODE_INCREMENT;
    DMA_InitStruct.DstTransferWidth = DMA_TR_WIDTH_8BITS;
    DMA_InitStruct.DstBurstLength = DMA_BURST_LENGTH_1;
    DMA_InitStruct.DstLinkedListCtrl = DISABLE;
    DMA_InitStruct.DstHandshake = DMA_HS_HARDWARE;
    DMA_InitStruct.DstHWHandshakePolarity = DMA_HS_POLARITY_HIGH;
    DMA_InitStruct.DstHWHandshakeInterface = 0U;

    DMA_InitStruct.Direction = DMA_DIR_PERIPH_TO_MEMORY;
    DMA_InitStruct.Priority = DMA_PRIORITY_7;
    DMA_InitStruct.FIFOMode = DMA_FIFO_MODE_0;
    DMA_InitStruct.LinkedListPointer = 0x0UL;
    DMA_InitStruct.NbData = sizeof(UART_RxBuffer);

    /* Init DMA channel */
    DMA_Init(DMA0, DMA_CH2, &DMA_InitStruct);

    /* Disable DMA channel interrupts */
    DMA_DisableIT(DMA0, DMA_CH2);
    DMA_EnableIT(DMA0, DMA_CH2);

    /* Enable DMA */
    DMA_Enable(DMA0);

    DMA_EnableIT_IntBlock(DMA0, DMA_CH2);

    /* Clear any pending interrupts */
    DMA_ClearIT_IntTfr(DMA0, DMA_CH2);
    DMA_ClearIT_IntBlock(DMA0, DMA_CH2);
    DMA_ClearIT_IntSrcTran(DMA0, DMA_CH2);
    DMA_ClearIT_IntDstTran(DMA0, DMA_CH2);
    DMA_ClearIT_IntErr(DMA0, DMA_CH2);
}

void DMA0_Channel0_Init(void)
{
    /* Deinit DMA channel */
    DMA_DeInit(DMA0, DMA_CH0);

    /* Configure DMA transfer */
    DMA_InitStruct_TypeDef DMA_InitStruct;
    DMA_StructInit(&DMA_InitStruct);

    DMA_InitStruct.SrcAddress = (uint32_t)UART_RxBuffer;
    DMA_InitStruct.SrcMode = DMA_MODE_NORMAL;
    DMA_InitStruct.SrcIncMode = DMA_INC_MODE_INCREMENT;
    DMA_InitStruct.SrcTransferWidth = DMA_TR_WIDTH_8BITS;
    DMA_InitStruct.SrcBurstLength = DMA_BURST_LENGTH_1;
    DMA_InitStruct.SrcLinkedListCtrl = DISABLE;
    DMA_InitStruct.SrcHandshake = DMA_HS_HARDWARE;
    DMA_InitStruct.SrcHWHandshakePolarity = DMA_HS_POLARITY_HIGH;
    DMA_InitStruct.SrcHWHandshakeInterface = 1U;

    DMA_InitStruct.DstAddress = UART_DMA_GetRegAddr(BSP_UART_INSTANCE);
    DMA_InitStruct.DstMode = DMA_MODE_NORMAL;
    DMA_InitStruct.DstIncMode = DMA_INC_MODE_NO_CHANGE;
    DMA_InitStruct.DstTransferWidth = DMA_TR_WIDTH_8BITS;
    DMA_InitStruct.DstBurstLength = DMA_BURST_LENGTH_1;
    DMA_InitStruct.DstLinkedListCtrl = DISABLE;
    DMA_InitStruct.DstHandshake = DMA_HS_HARDWARE;
    DMA_InitStruct.DstHWHandshakePolarity = DMA_HS_POLARITY_HIGH;
    DMA_InitStruct.DstHWHandshakeInterface = 8U;

    DMA_InitStruct.Direction = DMA_DIR_MEMORY_TO_PERIPH;
    DMA_InitStruct.Priority = DMA_PRIORITY_7;
    DMA_InitStruct.FIFOMode = DMA_FIFO_MODE_0;
    DMA_InitStruct.LinkedListPointer = 0x0UL;
    DMA_InitStruct.NbData = sizeof(UART_RxBuffer);

    /* Init DMA channel */
    DMA_Init(DMA0, DMA_CH0, &DMA_InitStruct);

    /* Disable DMA channel interrupts */
    DMA_DisableIT(DMA0, DMA_CH0);

    /* Enable DMA */
    DMA_Enable(DMA0);

    DMA_EnableIT_IntBlock(DMA0, DMA_CH0);
    DMA_EnableIT(DMA0, DMA_CH0);

    /* Clear any pending interrupts */
    DMA_ClearIT_IntTfr(DMA0, DMA_CH0);
    DMA_ClearIT_IntBlock(DMA0, DMA_CH0);
    DMA_ClearIT_IntSrcTran(DMA0, DMA_CH0);
    DMA_ClearIT_IntDstTran(DMA0, DMA_CH0);
    DMA_ClearIT_IntErr(DMA0, DMA_CH0);

    /* Configure timer interrupt in CLIC */
    CLIC_ConfigIRQ(CLIC_DMA0_IRQn,                 /* Interrupt */
        CLIC_INTATTR_MODE_MACHINE,       /* Privilege mode */
        1U,                              /* Level */
        1U,                              /* Priority */
        CLIC_INTATTR_SHV_VECTORED,       /* Vector mode */
        CLIC_INTATTR_TRIG_TYPE_LEVEL,    /* Type */
        CLIC_INTATTR_TRIG_POL_P);        /* Polarity */

    CLIC_EnableIRQ(CLIC_DMA0_IRQn);
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

    /* Init CLIC */
    CLIC_Config(1U, 1U);
    CLIC_SetLevelThreshold(0U);

    /* Init UART */
    BSP_UART_Init();

    printf("UART DMA RX/TX Example \r\n");


    /* Init DMA RX channel and configure DMA transfer */
    DMA0_Channel2_Init();

    /* Init DMA TX channel and configure DMA transfer */
    DMA0_Channel0_Init();

    /* Enable DMA channel */
    DMA_EnableChannel(DMA0, DMA_CH2);

    /* Enable interrupts globally */
    __enable_irq();
}

int main(void)
{
    Init();

    for(;;) {
        printf("Enter %d characters: \r\n", UART_BUFFER_LENGTH);
        while (!transmission_comlete){};
        printf("\r\nReception and transmission completed\r\n");
        transmission_comlete = 0;
        DMA0_Channel0_Init();
    }

	return 0;
}

void __attribute__ ((interrupt)) DMA0_IRQHandler(void)
{
    if (DMA_IsActiveIT_IntBlock(DMA0, DMA_CH2))  //RX Complete
    {
        DMA_ClearIT_IntBlock(DMA0, DMA_CH2);
        /* Start TX, Enable DMA channel */
        DMA_EnableChannel(DMA0, DMA_CH0);
    }

    if (DMA_IsActiveIT_IntBlock(DMA0, DMA_CH0))  //TX Complete
    {
        DMA_ClearIT_IntBlock(DMA0, DMA_CH0);
        transmission_comlete = 1;
    }

}
