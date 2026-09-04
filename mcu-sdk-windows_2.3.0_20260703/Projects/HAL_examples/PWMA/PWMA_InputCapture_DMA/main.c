/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/PWMA/PWMA_InputCapture_DMA/main.c
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

volatile uint32_t dma_block_trf_cmpl_flag_channel1 = 0;

uint16_t pwm_data[10] =
{
    1000-1,
    2000-1,
    3000-1,
    4000-1,
    5000-1,
    6000-1,
    7000-1,
    8000-1,
    9000-1,
    1500-1,
};

uint32_t input_pwm_pulse_period[2];

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

    DMA_InitStruct.SrcAddress = (uint32_t)pwm_data;
    DMA_InitStruct.SrcMode = DMA_MODE_NORMAL;
    DMA_InitStruct.SrcIncMode = DMA_INC_MODE_INCREMENT;
    DMA_InitStruct.SrcTransferWidth = DMA_TR_WIDTH_16BITS;
    DMA_InitStruct.SrcBurstLength = DMA_BURST_LENGTH_1;
    DMA_InitStruct.SrcLinkedListCtrl = DISABLE;
    DMA_InitStruct.SrcHandshake = DMA_HS_HARDWARE;
    DMA_InitStruct.SrcHWHandshakePolarity = DMA_HS_POLARITY_HIGH;
    DMA_InitStruct.SrcHWHandshakeInterface = 0U;

    DMA_InitStruct.DstAddress = (uint32_t)&(PWMA2->CCR0);
    DMA_InitStruct.DstMode = DMA_MODE_NORMAL;
    DMA_InitStruct.DstIncMode = DMA_INC_MODE_NO_CHANGE;
    DMA_InitStruct.DstTransferWidth = DMA_TR_WIDTH_16BITS;
    DMA_InitStruct.DstBurstLength = DMA_BURST_LENGTH_1;
    DMA_InitStruct.DstLinkedListCtrl = DISABLE;
    DMA_InitStruct.DstHandshake = DMA_HS_HARDWARE;
    DMA_InitStruct.DstHWHandshakePolarity = DMA_HS_POLARITY_HIGH;
    DMA_InitStruct.DstHWHandshakeInterface = 14U;

    DMA_InitStruct.Direction = DMA_DIR_MEMORY_TO_PERIPH;
    DMA_InitStruct.Priority = DMA_PRIORITY_7;
    DMA_InitStruct.FIFOMode = DMA_FIFO_MODE_0;
    DMA_InitStruct.LinkedListPointer = 0x0UL;
    DMA_InitStruct.NbData = sizeof(pwm_data) / 2;

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

void DMA0_Channel1_Init(void)
{
    /* Deinit DMA channel */
    DMA_DeInit(DMA0, DMA_CH1);

    /* Configure DMA transfer */
    DMA_InitStruct_TypeDef DMA_InitStruct;
    DMA_StructInit(&DMA_InitStruct);

    DMA_InitStruct.SrcAddress = (uint32_t)&(PWMA0->CCR0);
    DMA_InitStruct.SrcMode = DMA_MODE_NORMAL;
    DMA_InitStruct.SrcIncMode = DMA_INC_MODE_INCREMENT;
    DMA_InitStruct.SrcTransferWidth = DMA_TR_WIDTH_16BITS;
    DMA_InitStruct.SrcBurstLength = DMA_BURST_LENGTH_4;
    DMA_InitStruct.SrcLinkedListCtrl = DISABLE;
    DMA_InitStruct.SrcHandshake = DMA_HS_HARDWARE;
    DMA_InitStruct.SrcHWHandshakePolarity = DMA_HS_POLARITY_HIGH;
    DMA_InitStruct.SrcHWHandshakeInterface = 12U;

    DMA_InitStruct.DstAddress = (uint32_t)input_pwm_pulse_period;
    DMA_InitStruct.DstMode = DMA_MODE_NORMAL;
    DMA_InitStruct.DstIncMode = DMA_INC_MODE_INCREMENT;
    DMA_InitStruct.DstTransferWidth = DMA_TR_WIDTH_16BITS;
    DMA_InitStruct.DstBurstLength = DMA_BURST_LENGTH_4;
    DMA_InitStruct.DstLinkedListCtrl = DISABLE;
    DMA_InitStruct.DstHandshake = DMA_HS_HARDWARE;
    DMA_InitStruct.DstHWHandshakePolarity = DMA_HS_POLARITY_HIGH;
    DMA_InitStruct.DstHWHandshakeInterface = 0U;

    DMA_InitStruct.Direction = DMA_DIR_PERIPH_TO_MEMORY;
    DMA_InitStruct.Priority = DMA_PRIORITY_7;
    DMA_InitStruct.FIFOMode = DMA_FIFO_MODE_0;
    DMA_InitStruct.LinkedListPointer = 0x0UL;
    DMA_InitStruct.NbData = sizeof(input_pwm_pulse_period) / 2;

    /* Init DMA channel */
    DMA_Init(DMA0, DMA_CH1, &DMA_InitStruct);

    /* Disable DMA channel interrupts */
    DMA_DisableIT(DMA0, DMA_CH1);

    /* Enable DMA */
    DMA_Enable(DMA0);

    DMA_EnableIT_IntBlock(DMA0, DMA_CH1);
    DMA_EnableIT(DMA0, DMA_CH1);

    /* Clear any pending interrupts */
    DMA_ClearIT_IntTfr(DMA0, DMA_CH1);
    DMA_ClearIT_IntBlock(DMA0, DMA_CH1);
    DMA_ClearIT_IntSrcTran(DMA0, DMA_CH1);
    DMA_ClearIT_IntDstTran(DMA0, DMA_CH1);
    DMA_ClearIT_IntErr(DMA0, DMA_CH1);

    /* CLIC Interrupts are configured in DMA0_Channel0_Init()*/
}

void PWMA2_Init(void)
{
    PWMA_InitStruct_TypeDef PWMA_InitStruct = {0};
    PWMA_OC_InitStruct_TypeDef PWMA_OC_InitStruct = {0};
    PWMA_BDTR_InitStruct_TypeDef PWMA_BDTRInitStruct = {0};

    /* Enable GPIO2 clock */
    CRU_APB2_EnableClock(CRU_APB2_PERIPH_GPIO2);

    GPIO_SetPinMode(GPIO2, GPIO_PIN_0, GPIO_MODE_OUTPUT);
    GPIO_SetPinMode(GPIO2, GPIO_PIN_1, GPIO_MODE_OUTPUT);

    /* Configure alternate function of PWMA0 pins */
    CRU_SetPinAF(CRU_PORT_C, CRU_PIN_0, CRU_PIN_AF_2);    /* PWMA2_CH0P */
    CRU_SetPinAF(CRU_PORT_C, CRU_PIN_1, CRU_PIN_AF_2);    /* PWMA2_CH0N */

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
    PWMA_OC_InitStruct.OCMode = PWMA_OC_MODE_PWM1;
    PWMA_OC_InitStruct.OCState = PWMA_OC_STATE_DISABLE;
    PWMA_OC_InitStruct.OCNState = PWMA_OC_STATE_DISABLE;
    PWMA_OC_InitStruct.CompareValue = 1000 - 1;
    PWMA_OC_InitStruct.OCPolarity = PWMA_OC_POLARITY_HIGH;
    PWMA_OC_InitStruct.OCNPolarity = PWMA_OC_POLARITY_HIGH;
    PWMA_OC_InitStruct.OCIdleState = PWMA_OC_IDLESTATE_LOW;
    PWMA_OC_InitStruct.OCNIdleState = PWMA_OC_IDLESTATE_LOW;
    PWMA_OC_Init(PWMA2, PWMA_CH0, &PWMA_OC_InitStruct);
    PWMA_SetTriggerOutput(PWMA2, PWMA_TRGO_RESET);

    /* Init Break and Dead Time */
    PWMA_BDTR_StructInit(&PWMA_BDTRInitStruct);
    PWMA_BDTRInitStruct.OSSRState = PWMA_OSSR_DISABLE;
    PWMA_BDTRInitStruct.DeadTime = 0;
    PWMA_BDTRInitStruct.BreakState = PWMA_BRK_DISABLE;
    PWMA_BDTRInitStruct.BreakPolarity = PWMA_BRK_POLARITY_HIGH;
    PWMA_BDTRInitStruct.AutomaticOutput = PWMA_AUTOMATICOUTPUT_DISABLE;
    PWMA_BDTR_Init(PWMA2, &PWMA_BDTRInitStruct);

    /* Main output enable  */
    PWMA_EnableAllOutputs(PWMA2);

    /* Clear Update flag */
    //PWMA_ClearFlag_UPDATE(PWMA2);

    /* Enable Counter Update DMA request */
    PWMA_EnableDMAReq_UPDATE(PWMA2);

    /* Enable update interrupt */
    PWMA_EnableIT_UPDATE(PWMA2);

    /* Enable timer channel Compare interrupt */
    PWMA_EnableIT_CC0(PWMA2);

    /* Configure timer interrupt in CLIC */
    CLIC_ConfigIRQ(CLIC_PWMA2_IRQn,                 /* Interrupt */
                   CLIC_INTATTR_MODE_MACHINE,       /* Privilege mode */
                   1U,                              /* Level */
                   1U,                              /* Priority */
                   CLIC_INTATTR_SHV_VECTORED,       /* Vector mode */
                   CLIC_INTATTR_TRIG_TYPE_LEVEL,    /* Type */
                   CLIC_INTATTR_TRIG_POL_P);        /* Polarity */

    CLIC_EnableIRQ(CLIC_PWMA2_IRQn);
}

void PWMA0_Init(void)
{
    PWMA_InitStruct_TypeDef PWMA_InitStruct = {0};

    /* Enable GPIO0 clock */
    CRU_APB0_EnableClock(CRU_APB0_PERIPH_GPIO0);

    /* GPIO input mode*/
    GPIO_SetPinMode(GPIO0, GPIO_PIN_8, GPIO_MODE_INPUT);

    /* Enable input buffer */
    CRU_EnablePinInput(CRU_PORT_A, CRU_PIN_8);

    /* Configure alternate function of PWMA0 pins */
    CRU_SetPinAF(CRU_PORT_A, CRU_PIN_8, CRU_PIN_AF_4);    /* PWMA0_CH0P */

    PWMA_DeInit(PWMA0);

    /* Enable PWMA0 clock */
    CRU_APB2_EnableClock(CRU_APB2_PERIPH_PWMA0);

    /* Init timer */
    PWMA_StructInit(&PWMA_InitStruct);
    PWMA_InitStruct.Prescaler = 9999;
    PWMA_InitStruct.CounterMode = PWMA_COUNTERMODE_UP;
    PWMA_InitStruct.Autoreload = 19999;
    PWMA_InitStruct.ClockDivision = PWMA_CLOCKDIVISION_DIV1;
    PWMA_InitStruct.RepetitionCounter = 0;
    PWMA_Init(PWMA0, &PWMA_InitStruct);
    PWMA_EnableARRPreload(PWMA0);
    PWMA_SetClockSource(PWMA0, PWMA_CLOCKSOURCE_INTERNAL);
    PWMA_SetTriggerInput(PWMA0, PWMA_TS_TI1FP1);
    PWMA_SetSlaveMode(PWMA0, PWMA_SLAVEMODE_RESET);

    /* Init timer channel in Input Capture mode */
    PWMA_IC_SetActiveInput(PWMA0, PWMA_CH0, PWMA_IC_ACTIVEINPUT_DIRECTTI);
    PWMA_IC_SetPolarity(PWMA0, PWMA_CH0, PWMA_IC_POLARITY_RISING);
    PWMA_IC_SetFilter(PWMA0, PWMA_CH0, PWMA_IC_FILTER_FDIV1);
    PWMA_IC_SetPrescaler(PWMA0, PWMA_CH0, PWMA_IC_PRESCALER_DIV1);

    PWMA_IC_SetActiveInput(PWMA0, PWMA_CH1, PWMA_IC_ACTIVEINPUT_INDIRECTTI);
    PWMA_IC_SetPolarity(PWMA0, PWMA_CH1, PWMA_IC_POLARITY_FALLING);
    PWMA_IC_SetPrescaler(PWMA0, PWMA_CH1, PWMA_IC_PRESCALER_DIV1);
    PWMA_IC_SetFilter(PWMA0, PWMA_CH1, PWMA_IC_FILTER_FDIV1);

    PWMA_DisableIT_TRIG(PWMA0);
    PWMA_DisableDMAReq_TRIG(PWMA0);
    PWMA_SetTriggerOutput(PWMA0, PWMA_TRGO_RESET);

    /* Enable Capture DMA request */
    PWMA_EnableDMAReq_CC1(PWMA0);

    /* Enable update interrupt */
    PWMA_EnableIT_UPDATE(PWMA0);

    /* Enable timer channel Compare interrupt */
    PWMA_EnableIT_CC0(PWMA0);

    /* Configure timer interrupt in CLIC */
    CLIC_ConfigIRQ(CLIC_PWMA0_IRQn,                 /* Interrupt */
                   CLIC_INTATTR_MODE_MACHINE,       /* Privilege mode */
                   1U,                              /* Level */
                   1U,                              /* Priority */
                   CLIC_INTATTR_SHV_VECTORED,       /* Vector mode */
                   CLIC_INTATTR_TRIG_TYPE_LEVEL,    /* Type */
                   CLIC_INTATTR_TRIG_POL_P);        /* Polarity */

    CLIC_EnableIRQ(CLIC_PWMA0_IRQn);
}


void Init(void)
{
    /* Prepare eFlash to 100 MHz CCLK. See PLL configuration */
    EFLASH_Init(100000000);

    /* Init clock */
    CLK_Init();

    /* Init serial interface */
    bsp_serial_init();

    printf("PWMA DMA PWM mode example\r\n");
    
    /* Init CLIC */
    CLIC_Config(1U, 1U);
    CLIC_SetLevelThreshold(0U);

    /* Init LED */
    bsp_led_init();

    /* Init PWMA output*/
    PWMA2_Init();
    PWMA_CC_EnableChannel(PWMA2, PWMA_CH0 | PWMA_CH0N);

    /* Init PWMA input*/
    PWMA0_Init();
    PWMA_CC_EnableChannel(PWMA0, PWMA_CH0 | PWMA_CH1);

    /* Init DMA0 cnannel 0 */
    DMA0_Channel0_Init();

    /* Init DMA0 cnannel 1 */
    DMA0_Channel1_Init();

    /* Enable DMA channel */
    DMA_EnableChannel(DMA0, DMA_CH0);

    /* Enable DMA channel */
    DMA_EnableChannel(DMA0, DMA_CH1);

    /* Enable interrupts globally */
    __enable_irq();
}

int main(void)
{
    /* Init hardware */
    Init();

    /* Enable timer for PWM input*/
    PWMA_EnableCounter(PWMA0);

    /* Enable timer for PWM output*/
    PWMA_EnableCounter(PWMA2);

    for(;;) {
        /* Whait while DMA0, channel 1 block transfet finish*/
        while(dma_block_trf_cmpl_flag_channel1 == 0){};

        /* Reset transfer comlete flag */
        dma_block_trf_cmpl_flag_channel1 = 0;

        printf("Pulse = %ld\r\nPeriod = %ld\r\n\r\n", input_pwm_pulse_period[1] + 1, input_pwm_pulse_period[0] + 1);
    }

	return 0;
}

void __attribute__ ((interrupt)) PWMA0_IRQHandler(void)
{
    if (PWMA_IsActiveFlag_UPDATE(PWMA0))
    {
        /* Clear timer update interrupt flag */
        PWMA_ClearFlag_UPDATE(PWMA0);
    }

    if (PWMA_IsActiveFlag_CC0(PWMA0))
    {
        /* Clear channel Compare interrupt flag */
        PWMA_ClearFlag_CC0(PWMA0);
    }

    /* Clear interrupt */
    PWMA_ClearIT(PWMA0);
}

void __attribute__ ((interrupt)) PWMA2_IRQHandler(void)
{
    if (PWMA_IsActiveFlag_UPDATE(PWMA2))
    {
        /* Switch LED on */
        bsp_led_on();

        /* Clear timer update interrupt flag */
        PWMA_ClearFlag_UPDATE(PWMA2);
    }

    if (PWMA_IsActiveFlag_CC0(PWMA2))
    {
        bsp_led_off();
        /* Clear channel Compare interrupt flag */
        PWMA_ClearFlag_CC0(PWMA2);
    }

    /* Clear interrupt */
    PWMA_ClearIT(PWMA2);
}

void __attribute__ ((interrupt)) DMA0_IRQHandler(void)
{
    if (DMA_IsActiveIT_IntBlock(DMA0, DMA_CH0))
    {
        DMA_ClearIT_IntBlock(DMA0, DMA_CH0);

        /* Reinit DMA channel */
        DMA0_Channel0_Init();

        /* Enable DMA channel */
        DMA_EnableChannel(DMA0, DMA_CH0);
    }

    if (DMA_IsActiveIT_IntBlock(DMA0, DMA_CH1))
    {
        DMA_ClearIT_IntBlock(DMA0, DMA_CH1);

        /* Set transfer comlete flag */
        dma_block_trf_cmpl_flag_channel1 = 1;

        /* Reinit DMA channel */
        DMA0_Channel1_Init();

        /* Enable DMA channel */
        DMA_EnableChannel(DMA0, DMA_CH1);
    }
}
