/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/SPI/SPI_Master_Tx_DMA/main.c
 *  @author     Baikal electronics SDK team
 *  @brief      DMA SRAM->SPI block mode (no linkedlist) example source file
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
#include "bmcu_common.h"
#include "bmcu_cru.h"
#include "bmcu_spi.h"
#include "bmcu_dma.h"
#include "bmcu_eflash.h"

#ifndef PERIOD
#define PERIOD     (1000U)      //!< 1 second - preiod of the main loop
#endif
#define DMA_TX_NUM (255)        //!< Maximum DMA block transfer size (items)

/** 
 * @brief   Array to be transfered by DMA mechanism.
 * @details Size (in bytes) = DMA_TX_NUM * sizeof(uint32_t) = 255 * 4 = 1020
 *          SRAM must be used as a RAM region in this example project.
 *          So an explicitly uninitialized global array will be located by
 *          linker in the ".bss" section (SRAM).
 * @note    DMA source/destination address belong to the TCMA or TCMB memory
 *          region, then Front port addressing mode should be used.
 *          See "Memory" Application Note for more information.
*/
static uint32_t TxBuffer[DMA_TX_NUM];


/**
 * @brief   MCU clock configuration
 * @details Only CCLK and PCLK0 clock do matter for this project (MCU Core0 uses
 *          CCLK and SPI1 uses PCLK0). All available clocks are configured for
 *          the sake of completeness.
 */
static void
clk_init (void) {
    CRU_PLL_BypassMode_Disable();

    /** Set PLL source clock to C0 */
    CRU_PLL_SetSource(CRU_PLL_SRC_C0);

    /** PLL: 200 MHz */
    CRU_PLL_ForceReset();
    CRU_PLL_Config(1UL, 48UL, 6UL, 48UL);
    CRU_PLL_ReleaseReset();

    /** PLL lock time min. 500 cycles */
    __delay_cycles(500UL);

    /** Configure CCLK */
    CRU_SetCCLKPrescaler(CRU_CLK_DIV_1);
    CRU_CCLKPrescaler_Enable();
    CRU_SetCCLKSource(CRU_CLK_SRC_PLL);

    /** PCLK0: 100 MHz */
    CRU_SetPCLK0Prescaler(CRU_CLK_DIV_2);
    CRU_PCLK0Prescaler_Enable();
    CRU_SetPCLK0Source(CRU_CLK_SRC_PLL);

    /** PCLK1: 100 MHz */
    CRU_SetPCLK1Prescaler(CRU_CLK_DIV_2);
    CRU_PCLK1Prescaler_Enable();
    CRU_SetPCLK1Source(CRU_CLK_SRC_PLL);

    /** PCLK2: 100 MHz */
    CRU_SetPCLK2Prescaler(CRU_CLK_DIV_2);
    CRU_PCLK2Prescaler_Enable();
    CRU_SetPCLK2Source(CRU_CLK_SRC_PLL);

    /** HCLK: 100 MHz */
    CRU_SetHCLKPrescaler(CRU_CLK_DIV_2);
    CRU_HCLKPrescaler_Enable();
    CRU_SetHCLKSource(CRU_CLK_SRC_PLL);

    /** Used by system 1 MHz timer */
    CRU_SetCCLK1MHzPrescaler(199UL);
    CRU_SetHCLK1MHzPrescaler(99UL);
}


/**
 * @brief   SPI interface configuratioin
 * @details Set MCU SPI1-related pins to alternate mode.
 *          Set SPI1 (Master) interface to Tx mode with 32-bit data frame size.
 *          (SPI clock) = (APB0 clock) / (SPI clock divider) = 100 / 2 = 50 MHz
 */
static void
spi_init (void) {
    CRU_PIN_InitStruct_TypeDef CRU_PIN_InitStruct;
    SPI_InitStruct_TypeDef SPI_InitStruct;
    
    CRU_APB0_EnableClock(CRU_APB0_PERIPH_SPI1);
    
    CRU_PIN_StructInit(&CRU_PIN_InitStruct);
    CRU_PIN_InitStruct.Port      = CRU_PORT_A;
    CRU_PIN_InitStruct.Alternate = CRU_PIN_AF_1;    //!< SPI1 (Master)
    
    CRU_PIN_InitStruct.Pin = CRU_PIN_8;             //!< PA8 - CS
    CRU_PIN_Init(&CRU_PIN_InitStruct);
    
    CRU_PIN_InitStruct.Pin = CRU_PIN_9;             //!< PA9 - SCK
    CRU_PIN_Init(&CRU_PIN_InitStruct);
    
    CRU_PIN_InitStruct.Pin = CRU_PIN_10;            //!< PA10 - MOSI
    CRU_PIN_Init(&CRU_PIN_InitStruct);
    
    CRU_PIN_InitStruct.Pin = CRU_PIN_11;            //!< PA11 - MISO
    CRU_PIN_Init(&CRU_PIN_InitStruct);
    
    SPI_StructInit(&SPI_InitStruct);
    SPI_InitStruct.XferMode      = SPI_XFER_TX;
    SPI_InitStruct.DataFrameSize = SPI_DATA_FRAME_SIZE_32;
    SPI_InitStruct.ClkDivider    = 2;
    /** Another SPI_InitStruct struct fields (Polarity, Phase, SlaveSel,
     * RxSampleDelay, etc. will be set to default values */
    SPI_Init(SPI1, &SPI_InitStruct);
    
    /** Define SPI Tx FIFO level when DMA request will be sent.
     * Tx FIFO buffer depth is 8 entries. DMA burst length will be set to the
     * value 4 (see dma_config() function). Therefore, at the moment when SPI
     * Tx FIFO contains <= 4 entries, DMA request will be generated, 4 new
     * values will be copied and the SPI Tx FIFO will be completely filled. 
     */
    SPI_SetDMATxDataLvl(SPI1, 4);
    SPI_EnableDMATx(SPI1);
    
    SPI_Enable(SPI1);

    SPI_SetSlaveSelect(SPI1, SPI_SSEL_SELECTED);

    /** SPI peripheral block itself is enabled and Slave Select line is
     * configured. SPI transmission will begin when Tx FIFO buffer is filled. */
}


/** 
 * @brief   DMA configuration
 * @details Setup DMA 0 Channel 0 to perform TxBuffer -> SPI transfers.
 * @note    Source address (SrcAddress), transfer data width
 *          (SrcTransferWidth) and block length (NbData) must comply to the
 *          source buffer properties.
 *          Destionation address (DstAddress), DMA burst length
 *          (DstBurstLength) and hardware handshake number
 *          (DstHWHandshakeInterface) must be set according to the SPI interface
 *          parameters.
 */
static void
dma_config (void) {
    DMA_InitStruct_TypeDef DMA_InitStruct;

    DMA_DeInit(DMA0, DMA_CH0);
    DMA_StructInit(&DMA_InitStruct);

    DMA_InitStruct.SrcAddress              = (uint32_t)TxBuffer;
    DMA_InitStruct.SrcMode                 = DMA_MODE_NORMAL;
    DMA_InitStruct.SrcIncMode              = DMA_INC_MODE_INCREMENT;
    DMA_InitStruct.SrcTransferWidth        = DMA_TR_WIDTH_32BITS;
    DMA_InitStruct.SrcBurstLength          = DMA_BURST_LENGTH_4;
    DMA_InitStruct.SrcLinkedListCtrl       = DISABLE;
    DMA_InitStruct.SrcHandshake            = DMA_HS_HARDWARE;
    DMA_InitStruct.SrcHWHandshakePolarity  = DMA_HS_POLARITY_HIGH;
    DMA_InitStruct.SrcHWHandshakeInterface = 0U;

    DMA_InitStruct.DstAddress              = (uint32_t)&SPI1->DR[0];
    DMA_InitStruct.DstMode                 = DMA_MODE_NORMAL;
    DMA_InitStruct.DstIncMode              = DMA_INC_MODE_NO_CHANGE;
    DMA_InitStruct.DstTransferWidth        = DMA_TR_WIDTH_32BITS;
    DMA_InitStruct.DstBurstLength          = DMA_BURST_LENGTH_4;
    DMA_InitStruct.DstLinkedListCtrl       = DISABLE;
    DMA_InitStruct.DstHandshake            = DMA_HS_HARDWARE;
    DMA_InitStruct.DstHWHandshakePolarity  = DMA_HS_POLARITY_HIGH;
    DMA_InitStruct.DstHWHandshakeInterface = 4U;

    DMA_InitStruct.Direction               = DMA_DIR_MEMORY_TO_PERIPH;
    DMA_InitStruct.Priority                = DMA_PRIORITY_7;
    DMA_InitStruct.FIFOMode                = DMA_FIFO_MODE_0;
    DMA_InitStruct.LinkedListPointer       = 0x0UL;
    DMA_InitStruct.NbData                  = sizeof(TxBuffer) / sizeof(TxBuffer[0]);
    
    DMA_Init(DMA0, DMA_CH0, &DMA_InitStruct);

    /** DMA ISRs can be used for DMA configuration update or debug purposes. */
#ifdef USE_IRQ
    DMA_ClearIT_IntTfr(DMA0, DMA_CH0);
    DMA_ClearIT_IntBlock(DMA0, DMA_CH0);
    DMA_ClearIT_IntSrcTran(DMA0, DMA_CH0);
    DMA_ClearIT_IntDstTran(DMA0, DMA_CH0);
    DMA_ClearIT_IntErr(DMA0, DMA_CH0);

    DMA_EnableIT_IntTfr(DMA0, DMA_CH0);
    DMA_EnableIT_IntBlock(DMA0, DMA_CH0);
    DMA_EnableIT_IntSrcTran(DMA0, DMA_CH0);
    DMA_EnableIT_IntDstTran(DMA0, DMA_CH0);
    DMA_EnableIT_IntErr(DMA0, DMA_CH0);
    DMA_EnableIT(DMA0, DMA_CH0);
#endif

    DMA_Enable(DMA0);
}


int
main (void) {
    /** Prepare eFlash to 200 MHz CCLK. See PLL configuration */
    EFLASH_Init(200000000);

    clk_init();

    /** Write the TxBuffer array as an array of bytes. */
    for (size_t i = 0; i <= sizeof(TxBuffer); ++i) {
        *((uint8_t *)TxBuffer + i) = (uint8_t)i;
    }

#ifdef USE_IRQ
    CLIC_Config(1U, 1U);
    CLIC_SetLevelThreshold(0U);
    CLIC_ConfigIRQ(CLIC_DMA0_IRQn,               //!< Interrupt
                CLIC_INTATTR_MODE_MACHINE,       //!< Privilege mode
                1U,                              //!< Level
                1U,                              //!< Priority
                CLIC_INTATTR_SHV_VECTORED,       //!< Vector mode
                CLIC_INTATTR_TRIG_TYPE_LEVEL,    //!< Type
                CLIC_INTATTR_TRIG_POL_P);        //!< Polarity

    CLIC_EnableIRQ(CLIC_DMA0_IRQn);
    __enable_irq();
#endif

    spi_init();
    dma_config();

    for (;;) {
        /** Re-configure DMA source address since it was incremented during 
         * sending a previous DMA block.
         * All other DMA parameters remain the same. */
        DMA_SetSrcAddress(DMA0, DMA_CH0, (uint32_t)TxBuffer);
        /** Restart DMA transaction. */
        DMA_EnableChannel(DMA0, DMA_CH0);
        /** Wait until the first DMA transfer complete (SRAM->SPI) to refill 
         * SPI Tx FIFO. SPI is enabled and and SPI Slave Select line is already
         * configured (see spi_init()). SPI transaction will be started when 
         * SPI Tx FIFO is not empty. */

        /** DMA SRAM->SPI data transfer in progress. */
        
        /** Wait until SPI transfer is complete. */
        while (!SPI_IsActiveFlag(SPI1, SPI_FLAG_TFE));
        while (SPI_IsActiveFlag(SPI1, SPI_FLAG_BUSY));

        __delay_ms(PERIOD);
    }

	return 0;
}


/** DMA ISRs can be used for DMA configuration update or debug purposes. */
#ifdef USE_IRQ
void __attribute__ ((interrupt))
DMA0_IRQHandler (void) {
    if (DMA_IsActiveIT_IntTfr(DMA0, DMA_CH0)) {
        /** Add IntTfr flag processing here */
        DMA_ClearIT_IntTfr(DMA0, DMA_CH0);
    }

    if (DMA_IsActiveIT_IntBlock(DMA0, DMA_CH0)) {
        /** Add IntBlock flag processing here */
        DMA_ClearIT_IntBlock(DMA0, DMA_CH0);
    }

    if (DMA_IsActiveIT_IntSrcTran(DMA0, DMA_CH0)) {
        /** Add IntSrcTran flag processing here */
        DMA_ClearIT_IntSrcTran(DMA0, DMA_CH0);
    }

    if (DMA_IsActiveIT_IntDstTran(DMA0, DMA_CH0)) {
        /** Add IntDstTran flag processing here */
        DMA_ClearIT_IntDstTran(DMA0, DMA_CH0);
    }

    if (DMA_IsActiveIT_IntErr(DMA0, DMA_CH0)) {
        /** Add IntErr flag processing here */
        DMA_ClearIT_IntErr(DMA0, DMA_CH0);
    }
}
#endif
