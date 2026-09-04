/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/CLIC/CLIC_NestedInterrupts/main.c
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
#include <stdio.h>
#include "bmcu_common.h"
#include "bmcu_cru.h"
#include "bmcu_tim.h"

#include "bsp.h"

int
__io_putchar (int ch) {
    return bsp_serial_putchar(ch);
}


void
TIM0_Init (void) {
    /* Enable TIM0 clock */
    CRU_APB0_EnableClock(CRU_APB0_PERIPH_TIM0);

    /* Deinitialize timer channels */
    TIM_DeInit(TIM0, TIM_CH0);
    TIM_DeInit(TIM0, TIM_CH1);
    TIM_DeInit(TIM0, TIM_CH2);
    TIM_DeInit(TIM0, TIM_CH3);

    CRU_Clocks_TypeDef cru_clocks;
    CRU_GetSystemClocksFreq(&cru_clocks);

    /* Init Channel 0 */
    TIM_InitStruct_TypeDef TIM_InitStruct;
    TIM_StructInit(&TIM_InitStruct);
    TIM_InitStruct.CounterMode = TIM_COUNTERMODE_PERIODIC;
    TIM_InitStruct.LoadCount = cru_clocks.PCLK0_Frequency / 5U - 1UL;   /* 200ms */
    TIM_Init(TIM0, TIM_CH0, &TIM_InitStruct);

    TIM_EnableIT(TIM0, TIM_CH0);

    CLIC_ConfigIRQ(CLIC_TIM0_Channel0_IRQn,         /* Interrupt */
                   CLIC_INTATTR_MODE_MACHINE,       /* Privilege mode */
                   4U,                              /* Level */
                   0U,                              /* Priority */
                   CLIC_INTATTR_SHV_VECTORED,       /* Vector mode */
                   CLIC_INTATTR_TRIG_TYPE_EDGE,     /* Type */
                   CLIC_INTATTR_TRIG_POL_P);        /* Polarity */

    CLIC_EnableIRQ(CLIC_TIM0_Channel0_IRQn);

    /* Init Channel 1 */
    TIM_InitStruct.LoadCount = cru_clocks.PCLK0_Frequency / 2U - 1UL;   /* 500ms */
    TIM_Init(TIM0, TIM_CH1, &TIM_InitStruct);

    TIM_EnableIT(TIM0, TIM_CH1);

    CLIC_ConfigIRQ(CLIC_TIM0_Channel1_IRQn,         /* Interrupt */
                   CLIC_INTATTR_MODE_MACHINE,       /* Privilege mode */
                   3U,                              /* Level */
                   0U,                              /* Priority */
                   CLIC_INTATTR_SHV_VECTORED,       /* Vector mode */
                   CLIC_INTATTR_TRIG_TYPE_EDGE,     /* Type */
                   CLIC_INTATTR_TRIG_POL_P);        /* Polarity */

    CLIC_EnableIRQ(CLIC_TIM0_Channel1_IRQn);

    /* Init Channel 2 */
    TIM_InitStruct.LoadCount = cru_clocks.PCLK0_Frequency - 1UL;    /* 1s */
    TIM_Init(TIM0, TIM_CH2, &TIM_InitStruct);

    TIM_EnableIT(TIM0, TIM_CH2);

    CLIC_ConfigIRQ(CLIC_TIM0_Channel2_IRQn,         /* Interrupt */
                   CLIC_INTATTR_MODE_MACHINE,       /* Privilege mode */
                   2U,                              /* Level */
                   0U,                              /* Priority */
                   CLIC_INTATTR_SHV_VECTORED,       /* Vector mode */
                   CLIC_INTATTR_TRIG_TYPE_EDGE,     /* Type */
                   CLIC_INTATTR_TRIG_POL_P);        /* Polarity */

    CLIC_EnableIRQ(CLIC_TIM0_Channel2_IRQn);

    /* Init Channel 3 */
    TIM_InitStruct.LoadCount = cru_clocks.PCLK0_Frequency * 5U - 1UL;   /* 5s */
    TIM_Init(TIM0, TIM_CH3, &TIM_InitStruct);

    TIM_EnableIT(TIM0, TIM_CH3);

    CLIC_ConfigIRQ(CLIC_TIM0_Channel3_IRQn,         /* Interrupt */
                   CLIC_INTATTR_MODE_MACHINE,       /* Privilege mode */
                   1U,                              /* Level */
                   0U,                              /* Priority */
                   CLIC_INTATTR_SHV_VECTORED,       /* Vector mode */
                   CLIC_INTATTR_TRIG_TYPE_EDGE,     /* Type */
                   CLIC_INTATTR_TRIG_POL_P);        /* Polarity */

    CLIC_EnableIRQ(CLIC_TIM0_Channel3_IRQn);
}


void
Init (void) {
    /* Init serial interface */
    bsp_serial_init();

    /* Init timer */
    TIM0_Init();

    /* Init CLIC */
    CLIC_Config(1U, 3U);
    CLIC_SetLevelThreshold(0U);

    /* Enable interrupts globally */
    __enable_irq();
}


int
main (void) {
    Init();

    TIM_EnableChannel(TIM0, TIM_CH0);
    TIM_EnableChannel(TIM0, TIM_CH1);
    TIM_EnableChannel(TIM0, TIM_CH2);
    TIM_EnableChannel(TIM0, TIM_CH3);

    for(;;) {
        /* A bunch of NOPs to add some more variation to mepc value. Added for
           demonstration purpose. The reported mcause and mepc values must be
           equal on interrupt enter and exit. It guarantees, that the registers
           were restored properly after an interrupt was preempted. */
        __ASM volatile("nop");
        __ASM volatile("nop");
        __ASM volatile("nop");
        __ASM volatile("nop");
        __ASM volatile("nop");
    }

	return 0;
}


void __attribute__ ((interrupt))
TIM0_Channel0_IRQHandler (void) {
    /* Report the interrupt enter */
    unsigned long mcause = CSR_READ(CSR_MCAUSE);
    unsigned long mepc = CSR_READ(CSR_MEPC);
    printf("CH0: <enter>, mcause = 0x%08lX, mepc = 0x%08lX\r\n", mcause, mepc);

    ENTER_NESTED_INTERRUPT();

    /* Disable the timer channel */
    TIM_DisableChannel(TIM0, TIM_CH0);

    /* Clear the timer channel interrupt */
    TIM_ClearIT(TIM0, TIM_CH0);

    /* Simulate some long data processing. It is used only for demonstration
       purpose. Real-life interrupt should return as soon as possible. */
    __delay_ms(10UL);

    /* Restart the timer channel */
    TIM_EnableChannel(TIM0, TIM_CH0);

    EXIT_NESTED_INTERRUPT();

    /* Report the interrupt exit */
    mcause = CSR_READ(CSR_MCAUSE);
    mepc = CSR_READ(CSR_MEPC);
    printf("CH0: <exit>, mcause = 0x%08lX, mepc = 0x%08lX\r\n", mcause, mepc);
}


void __attribute__ ((interrupt))
TIM0_Channel1_IRQHandler (void) {
    /* Report the interrupt enter */
    unsigned long mcause = CSR_READ(CSR_MCAUSE);
    unsigned long mepc = CSR_READ(CSR_MEPC);
    printf("CH1: <enter>, mcause = 0x%08lX, mepc = 0x%08lX\r\n", mcause, mepc);

    ENTER_NESTED_INTERRUPT();

    /* Disable the timer channel */
    TIM_DisableChannel(TIM0, TIM_CH1);

    /* Clear the timer channel interrupt */
    TIM_ClearIT(TIM0, TIM_CH1);

    /* Simulate some long data processing. It is used only for demonstration
       purpose. Real-life interrupt should return as soon as possible. */
    __delay_ms(100UL);

    /* Restart the timer channel */
    TIM_EnableChannel(TIM0, TIM_CH1);

    EXIT_NESTED_INTERRUPT();

    /* Report the interrupt exit */
    mcause = CSR_READ(CSR_MCAUSE);
    mepc = CSR_READ(CSR_MEPC);
    printf("CH1: <exit>, mcause = 0x%08lX, mepc = 0x%08lX\r\n", mcause, mepc);
}

void __attribute__ ((interrupt))
TIM0_Channel2_IRQHandler (void) {
    /* Report the interrupt enter */
    unsigned long mcause = CSR_READ(CSR_MCAUSE);
    unsigned long mepc = CSR_READ(CSR_MEPC);
    printf("CH2: <enter>, mcause = 0x%08lX, mepc = 0x%08lX\r\n", mcause, mepc);

    ENTER_NESTED_INTERRUPT();

    /* Disable the timer channel */
    TIM_DisableChannel(TIM0, TIM_CH2);

    /* Clear the timer channel interrupt */
    TIM_ClearIT(TIM0, TIM_CH2);

    /* Simulate some long data processing. It is used only for demonstration
       purpose. Real-life interrupt should return as soon as possible. */
    __delay_ms(250UL);

    /* Restart the timer channel */
    TIM_EnableChannel(TIM0, TIM_CH2);

    EXIT_NESTED_INTERRUPT();

    /* Report the interrupt exit */
    mcause = CSR_READ(CSR_MCAUSE);
    mepc = CSR_READ(CSR_MEPC);
    printf("CH2: <exit>, mcause = 0x%08lX, mepc = 0x%08lX\r\n", mcause, mepc);
}

void __attribute__ ((interrupt))
TIM0_Channel3_IRQHandler (void) {
    /* Report the interrupt enter */
    unsigned long mcause = CSR_READ(CSR_MCAUSE);
    unsigned long mepc = CSR_READ(CSR_MEPC);
    printf("CH3: <enter>, mcause = 0x%08lX, mepc = 0x%08lX\r\n", mcause, mepc);

    ENTER_NESTED_INTERRUPT();

    /* Disable the timer channel */
    TIM_DisableChannel(TIM0, TIM_CH3);

    /* Clear the timer channel interrupt */
    TIM_ClearIT(TIM0, TIM_CH3);

    /* Simulate some long data processing. It is used only for demonstration
       purpose. Real-life interrupt should return as soon as possible. */
    __delay_ms(2500UL);

    /* Restart the timer channel */
    TIM_EnableChannel(TIM0, TIM_CH3);

    EXIT_NESTED_INTERRUPT();

    /* Report the interrupt exit */
    mcause = CSR_READ(CSR_MCAUSE);
    mepc = CSR_READ(CSR_MEPC);
    printf("CH3: <exit>, mcause = 0x%08lX, mepc = 0x%08lX\r\n", mcause, mepc);
}
