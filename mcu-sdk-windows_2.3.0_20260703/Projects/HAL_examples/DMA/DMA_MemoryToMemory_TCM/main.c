/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/DMA/DMA_MemoryToMemory_TCM/main.c
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
#include "bmcu_dma.h"

#include "bsp.h"

#define TX_BUFFER_LENGTH 128
#define RX_BUFFER_LENGTH 128

char TxBuffer[TX_BUFFER_LENGTH] __attribute__((section(".dma_data"))) __ALIGNED(4);
char RxBuffer[RX_BUFFER_LENGTH] __attribute__((section(".dma_data"))) __ALIGNED(4);
const char TestStr[] = "DMA Memory to Memory Transfer Example\r\n";

int __io_putchar(int ch)
{
    return bsp_serial_putchar(ch);
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

void DMA0_Channel0_Init(void)
{
    /* Deinit DMA channel */
    DMA_DeInit(DMA0, DMA_CH0);

    /* Configure DMA transfer */
    DMA_InitStruct_TypeDef DMA_InitStruct;
    DMA_StructInit(&DMA_InitStruct);

    DMA_InitStruct.SrcAddress = (uint32_t)TxBuffer + 0x10000000UL;
    DMA_InitStruct.SrcMode = DMA_MODE_NORMAL;
    DMA_InitStruct.SrcIncMode = DMA_INC_MODE_INCREMENT;
    DMA_InitStruct.SrcTransferWidth = DMA_TR_WIDTH_8BITS;
    DMA_InitStruct.SrcBurstLength = DMA_BURST_LENGTH_4;
    DMA_InitStruct.SrcLinkedListCtrl = DISABLE;
    DMA_InitStruct.SrcHandshake = DMA_HS_HARDWARE;
    DMA_InitStruct.SrcHWHandshakePolarity = DMA_HS_POLARITY_HIGH;
    DMA_InitStruct.SrcHWHandshakeInterface = 0U;

    DMA_InitStruct.DstAddress = (uint32_t)RxBuffer + 0x10000000UL;
    DMA_InitStruct.DstMode = DMA_MODE_NORMAL;
    DMA_InitStruct.DstIncMode = DMA_INC_MODE_INCREMENT;
    DMA_InitStruct.DstTransferWidth = DMA_TR_WIDTH_8BITS;
    DMA_InitStruct.DstBurstLength = DMA_BURST_LENGTH_4;
    DMA_InitStruct.DstLinkedListCtrl = DISABLE;
    DMA_InitStruct.DstHandshake = DMA_HS_HARDWARE;
    DMA_InitStruct.DstHWHandshakePolarity = DMA_HS_POLARITY_HIGH;
    DMA_InitStruct.DstHWHandshakeInterface = 0U;

    DMA_InitStruct.Direction = DMA_DIR_MEMORY_TO_MEMORY;
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
    EFLASH_Init(200000000);
    
    /* Init clock */
    CLK_Init();

    /* Init serial interface */
    bsp_serial_init();
}

int main(void)
{
    Init();

    for(;;)
    {
        memset(TxBuffer, '\0', TX_BUFFER_LENGTH);
        memset(RxBuffer, '\0', RX_BUFFER_LENGTH);

        strcpy(TxBuffer, TestStr);

        /* Init DMA channel and configure DMA transfer */
        DMA0_Channel0_Init();

        printf("Start DMA transfer\r\n");

        /* Enable DMA channel */
        DMA_EnableChannel(DMA0, DMA_CH0);

        /* Wait for DMA transfer to complete */
        while (DMA_IsActiveIT_IntTfr(DMA0, DMA_CH0) != 1UL);

        printf("DMA transfer complete\r\n");

        if (memcmp(TxBuffer, RxBuffer, TX_BUFFER_LENGTH) == 0)
        {
            printf("Buffers 0x%x (TX) and 0x%x (RX) are equal:\r\n\"%s\"\r\n",
                    (unsigned int)&TxBuffer, (unsigned int)&RxBuffer, TxBuffer);
        }
        else
        {
            printf("Buffers 0x%x (TX) and 0x%x (RX) are NOT equal:\r\nTX:\"%s\"\r\nRX:\"%s\"\r\n",
                    (unsigned int)&TxBuffer, (unsigned int)&RxBuffer, TxBuffer, RxBuffer);
        }

        __delay_ms(1000UL);
    }

	return 0;
}
