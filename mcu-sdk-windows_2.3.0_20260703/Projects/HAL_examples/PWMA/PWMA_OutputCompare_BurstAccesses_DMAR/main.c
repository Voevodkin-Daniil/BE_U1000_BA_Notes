/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/PWMA/PWMA_OutputCompare_BurstAccesses_DMAR/main.c
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

#include "bmcu_common.h"
#include "bmcu_cru.h"
#include "bmcu_gpio.h"
#include "bmcu_pwma.h"
#include "bmcu_dma.h"

#include "bsp.h"

uint32_t dma_block_trf_cnt = 0;
volatile uint32_t dma_block_trf_cmpl_flag =0;

uint32_t BurstDmaBuffer[8] = {
1000,
1500,
2000,
2500,
3000,
4000,
5000,
6000
};

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

    DMA_InitStruct.SrcAddress = (uint32_t)BurstDmaBuffer;
    DMA_InitStruct.SrcMode = DMA_MODE_NORMAL;
    DMA_InitStruct.SrcIncMode = DMA_INC_MODE_INCREMENT;
    DMA_InitStruct.SrcTransferWidth = DMA_TR_WIDTH_32BITS;
    DMA_InitStruct.SrcBurstLength = DMA_BURST_LENGTH_4;
    DMA_InitStruct.SrcLinkedListCtrl = DISABLE;
    DMA_InitStruct.SrcHandshake = DMA_HS_HARDWARE;
    DMA_InitStruct.SrcHWHandshakePolarity = DMA_HS_POLARITY_HIGH;
    DMA_InitStruct.SrcHWHandshakeInterface = 0U;

    DMA_InitStruct.DstAddress = (uint32_t)&(PWMA2->DMAR);
    DMA_InitStruct.DstMode = DMA_MODE_NORMAL;
    DMA_InitStruct.DstIncMode = DMA_INC_MODE_NO_CHANGE;
    DMA_InitStruct.DstTransferWidth = DMA_TR_WIDTH_32BITS;
    DMA_InitStruct.DstBurstLength = DMA_BURST_LENGTH_4;
    DMA_InitStruct.DstLinkedListCtrl = DISABLE;
    DMA_InitStruct.DstHandshake = DMA_HS_HARDWARE;
    DMA_InitStruct.DstHWHandshakePolarity = DMA_HS_POLARITY_HIGH;
    DMA_InitStruct.DstHWHandshakeInterface = 14U;

    DMA_InitStruct.Direction = DMA_DIR_MEMORY_TO_PERIPH;
    DMA_InitStruct.Priority = DMA_PRIORITY_7;
    DMA_InitStruct.FIFOMode = DMA_FIFO_MODE_0;
    DMA_InitStruct.LinkedListPointer = 0x0UL;
    DMA_InitStruct.NbData = sizeof(BurstDmaBuffer) / 4;

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

void PWMA2_Init(void)
{
    PWMA_InitStruct_TypeDef PWMA_InitStruct = {0};
    PWMA_OC_InitStruct_TypeDef PWMA_OC_InitStruct = {0};
    PWMA_BDTR_InitStruct_TypeDef PWMA_BDTRInitStruct = {0};

    /* Enable GPIO2 clock */
    CRU_APB2_EnableClock(CRU_APB2_PERIPH_GPIO2);

    GPIO_SetPinMode(GPIO2, GPIO_PIN_0, GPIO_MODE_OUTPUT);
    GPIO_SetPinMode(GPIO2, GPIO_PIN_2, GPIO_MODE_OUTPUT);
    GPIO_SetPinMode(GPIO2, GPIO_PIN_4, GPIO_MODE_OUTPUT);
    GPIO_SetPinMode(GPIO2, GPIO_PIN_11, GPIO_MODE_OUTPUT);

    /* Configure alternate function of PWMA0 pins */
    CRU_SetPinAF(CRU_PORT_C, CRU_PIN_0, CRU_PIN_AF_2);    /* PWMA2_CH0P */
    CRU_SetPinAF(CRU_PORT_C, CRU_PIN_2, CRU_PIN_AF_2);    /* PWMA2_CH1P */
    CRU_SetPinAF(CRU_PORT_C, CRU_PIN_4, CRU_PIN_AF_2);    /* PWMA2_CH2P */
    CRU_SetPinAF(CRU_PORT_C, CRU_PIN_11, CRU_PIN_AF_2);   /* PWMA2_CH3P */

    PWMA_DeInit(PWMA2);

    /* Enable PWMA2 clock */
    CRU_APB2_EnableClock(CRU_APB2_PERIPH_PWMA2);

    /* Init timer */
    PWMA_StructInit(&PWMA_InitStruct);
    PWMA_InitStruct.Autoreload = 9999U;
    PWMA_InitStruct.ClockDivision = PWMA_CLOCKDIVISION_DIV1;
    PWMA_InitStruct.CounterMode = PWMA_COUNTERMODE_UP;
    PWMA_InitStruct.Prescaler = 9999U;
    PWMA_InitStruct.RepetitionCounter = 0;
    PWMA_Init(PWMA2, &PWMA_InitStruct);
    PWMA_EnableARRPreload(PWMA2);
    PWMA_SetClockSource(PWMA2, PWMA_CLOCKSOURCE_INTERNAL);


    /* Init timer channel in Output Compare mode */
    PWMA_OC_StructInit(&PWMA_OC_InitStruct);
    PWMA_OC_EnablePreload(PWMA2, PWMA_CH0);
    PWMA_OC_EnablePreload(PWMA2, PWMA_CH1);
    PWMA_OC_EnablePreload(PWMA2, PWMA_CH2);
    PWMA_OC_EnablePreload(PWMA2, PWMA_CH3);
    PWMA_OC_InitStruct.OCMode = PWMA_OC_MODE_PWM1;
    PWMA_OC_InitStruct.OCState = PWMA_OC_STATE_DISABLE;
    PWMA_OC_InitStruct.OCNState = PWMA_OC_STATE_DISABLE;
    PWMA_OC_InitStruct.CompareValue = 1000 - 1;
    PWMA_OC_InitStruct.OCPolarity = PWMA_OC_POLARITY_HIGH;
    PWMA_OC_InitStruct.OCNPolarity = PWMA_OC_POLARITY_HIGH;
    PWMA_OC_InitStruct.OCIdleState = PWMA_OC_IDLESTATE_LOW;
    PWMA_OC_InitStruct.OCNIdleState = PWMA_OC_IDLESTATE_LOW;
    PWMA_OC_Init(PWMA2, PWMA_CH0, &PWMA_OC_InitStruct);
    PWMA_OC_Init(PWMA2, PWMA_CH1, &PWMA_OC_InitStruct);
    PWMA_OC_Init(PWMA2, PWMA_CH2, &PWMA_OC_InitStruct);
    PWMA_OC_Init(PWMA2, PWMA_CH3, &PWMA_OC_InitStruct);
    PWMA_SetTriggerOutput(PWMA2, PWMA_TRGO_RESET);

    /* Init Break and Dead Time */
    PWMA_BDTR_StructInit(&PWMA_BDTRInitStruct);
    PWMA_BDTRInitStruct.OSSRState = PWMA_OSSR_DISABLE;
    PWMA_BDTRInitStruct.DeadTime = 0;
    PWMA_BDTRInitStruct.BreakState = PWMA_BRK_DISABLE;
    PWMA_BDTRInitStruct.BreakPolarity = PWMA_BRK_POLARITY_HIGH;
    PWMA_BDTRInitStruct.AutomaticOutput = PWMA_AUTOMATICOUTPUT_DISABLE;
    PWMA_BDTR_Init(PWMA2, &PWMA_BDTRInitStruct);

    PWMA_DMA_Config(PWMA2, PWMA_DMABURST_BASEADDR_CCR0, PWMA_DMABURST_LENGTH_4TRANSFERS);

    /* Main output enable  */
    PWMA_EnableAllOutputs(PWMA2);

    /* Clear Update flag */
    PWMA_ClearFlag_UPDATE(PWMA2);

    /* Enable Counter Update DMA request */
    PWMA_EnableDMAReq_UPDATE(PWMA2);
}

void Init(void)
{
    /* Prepare eFlash to 100 MHz CCLK. See PLL configuration */
    EFLASH_Init(100000000);

    /* Init clock */
    CLK_Init();

    /* Init serial interface */
    bsp_serial_init();

    printf("PWMA timer in PWM mode example, use DMAR register \r\n");

    /* Init CLIC */
    CLIC_Config(1U, 1U);
    CLIC_SetLevelThreshold(0U);

    /* Init PWMA */
    PWMA2_Init();
    PWMA_CC_EnableChannel(PWMA2, PWMA_CH0 | PWMA_CH1 | PWMA_CH2 | PWMA_CH3);

    /* Init DMA0 cnannel 0 */
    DMA0_Channel0_Init();

    /* Enable DMA channel */
    DMA_EnableChannel(DMA0, DMA_CH0);

    /* Enable interrupts globally */
    __enable_irq();
}

int main(void)
{
    /* Init hardware */
    Init();

    /* Enable timer */
    PWMA_EnableCounter(PWMA2);

    for(;;) {
        /* Wait for DMA transfer to complete */
        while(dma_block_trf_cmpl_flag == 0) {};
        dma_block_trf_cmpl_flag = 0;
        dma_block_trf_cnt++;
        printf("DMA TX Block complete %ld\r\n", dma_block_trf_cnt);

        /* Reinit DMA channel */
        DMA0_Channel0_Init();

        /* Enable DMA channel */
        DMA_EnableChannel(DMA0, DMA_CH0);
    }

	return 0;
}

void __attribute__ ((interrupt)) DMA0_IRQHandler(void)
{
    if (DMA_IsActiveIT_IntBlock(DMA0, DMA_CH0))
    {
        DMA_ClearIT_IntBlock(DMA0, DMA_CH0);

        /* Enable DMA channel */
        dma_block_trf_cmpl_flag = 1;
    }
}
