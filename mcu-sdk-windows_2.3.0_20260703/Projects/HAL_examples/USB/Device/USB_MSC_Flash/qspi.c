/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/USB/Device/USB_MSC_Flash/qspi.c
 *  @author     Baikal electronics SDK team
 *  @brief      QSPI driver source file
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

#include "qspi.h"
#include "bmcu_cru.h"
#include "bmcu_dma.h"
#include "bmcu_gpio.h"
#include "bmcu_qspi.h"

#define DMA_LLI_ARRAY_SIZE              17U
#define DMA_LLI_BLOCK_SIZE              255U

#define DMA_TX_CHANNEL                  DMA_CH2
#define DMA_RX_CHANNEL                  DMA_CH3

DMA_LLI_TypeDef DMA_LLI_Chain[DMA_LLI_ARRAY_SIZE] __ALIGNED(4);

static void DMA1_Init(DMA_Channels_TypeDef Channel, DMA_InitStruct_TypeDef *DMA_InitStruct);
static void DMA1_SetupRx(uint8_t* DataBuf, uint32_t DataLen);
static void DMA1_SetupRx(uint8_t* DataBuf, uint32_t DataLen);

/**
 * @brief Initialize QSPI interface.
 */
void QSPI1_Init(void)
{
    /* Enable clock */
    CRU_APB1_EnableClock(CRU_APB1_PERIPH_QSPI1);

    /* Disable XIP mode */
    CRU_QSPI1_XIP_Disable();

    /* Configure pull-ups */
    CRU_SetPinPull(CRU_PORT_B, CRU_PIN_0, CRU_PIN_PULL_NO);
    CRU_SetPinPull(CRU_PORT_B, CRU_PIN_1, CRU_PIN_PULL_NO);
    CRU_SetPinPull(CRU_PORT_B, CRU_PIN_2, CRU_PIN_PULL_UP);
    CRU_SetPinPull(CRU_PORT_B, CRU_PIN_3, CRU_PIN_PULL_UP);
    CRU_SetPinPull(CRU_PORT_B, CRU_PIN_4, CRU_PIN_PULL_UP);
    CRU_SetPinPull(CRU_PORT_B, CRU_PIN_5, CRU_PIN_PULL_UP);

    /* Configure alternate function */
    CRU_SetPinAF(CRU_PORT_B, CRU_PIN_0, CRU_PIN_AF_3);  /* CS */
    CRU_SetPinAF(CRU_PORT_B, CRU_PIN_1, CRU_PIN_AF_3);  /* SCK */
    CRU_SetPinAF(CRU_PORT_B, CRU_PIN_2, CRU_PIN_AF_3);  /* IO0 */
    CRU_SetPinAF(CRU_PORT_B, CRU_PIN_3, CRU_PIN_AF_3);  /* IO1 */
    CRU_SetPinAF(CRU_PORT_B, CRU_PIN_4, CRU_PIN_AF_3);  /* IO2 */
    CRU_SetPinAF(CRU_PORT_B, CRU_PIN_5, CRU_PIN_AF_3);  /* IO3 */

    /* Configure pin drive strength */
    CRU_SetPinDriveStrength(CRU_PORT_B, CRU_PIN_0, CRU_PIN_DRIVE_STRENGTH_3);
    CRU_SetPinDriveStrength(CRU_PORT_B, CRU_PIN_1, CRU_PIN_DRIVE_STRENGTH_3);
    CRU_SetPinDriveStrength(CRU_PORT_B, CRU_PIN_2, CRU_PIN_DRIVE_STRENGTH_3);
    CRU_SetPinDriveStrength(CRU_PORT_B, CRU_PIN_3, CRU_PIN_DRIVE_STRENGTH_3);
    CRU_SetPinDriveStrength(CRU_PORT_B, CRU_PIN_4, CRU_PIN_DRIVE_STRENGTH_3);
    CRU_SetPinDriveStrength(CRU_PORT_B, CRU_PIN_5, CRU_PIN_DRIVE_STRENGTH_3);

    /* Reset QSPI registers */
    QSPI_DeInit(QSPI1);

    /* Init QSPI */
    QSPI_InitStruct_TypeDef QSPI_InitStruct;
    QSPI_StructInit(&QSPI_InitStruct);

    CRU_Clocks_TypeDef CRU_Clocks;
    CRU_GetSystemClocksFreq(&CRU_Clocks);

    QSPI_InitStruct.ClkDivider = 100U; /* Clock speed is limited by DMA latency here, not by QSPI device speed limit. */
                                       /* The Tx FIFO must be filled by the DMA faster than it shifts data out.       */
                                       /* The DMA should transfer data to memory faster that the Rx FIFO is filled.   */
    QSPI_InitStruct.AddrInstXferMode = QSPI_XFER_BOTH;
    QSPI_InitStruct.RxSampleDelay = 0U;

    QSPI_Init(QSPI1, &QSPI_InitStruct);

    QSPI_SetDMATxDataLvl(QSPI1, 7UL);
    QSPI_EnableDMATx(QSPI1);
    QSPI_SetDMARxDataLvl(QSPI1, 0UL);
    QSPI_EnableDMARx(QSPI1);

    QSPI_SetSlaveSelect(QSPI1, QSPI_SSEL_NOT_SELECTED);
}

/**
 * @brief Initialize DMA channel.
 * @param Channel The DMA channel. Can be one of DMA_Channels_TypeDef values.
 * @param DMA_InitStruct The pointer to DMA_InitStruct_TypeDef structure.
 */
void DMA1_Init(DMA_Channels_TypeDef Channel, DMA_InitStruct_TypeDef *DMA_InitStruct)
{
    /* Deinit DMA channel */
    DMA_DeInit(DMA1, Channel);

    /* Init DMA channel */
    DMA_Init(DMA1, Channel, DMA_InitStruct);

    /* Enable DMA */
    DMA_Enable(DMA1);
}

/**
 * @brief Set up DMA Tx transfer.
 * @note The function prepares multi-block data transfer,
 *       if the data requires more than one DMA block.
 * @param DataBuf The data buffer.
 * @param DataLen The data length.
 */
void DMA1_SetupTx(uint8_t* DataBuf, uint32_t DataLen)
{
    if (DataLen > 0UL)
    {
        DMA_InitStruct_TypeDef DMA_InitStruct;
        DMA_StructInit(&DMA_InitStruct);

        DMA_InitStruct.SrcAddress = (uint32_t)DataBuf;
        DMA_InitStruct.SrcMode = DMA_MODE_NORMAL;
        DMA_InitStruct.SrcIncMode = DMA_INC_MODE_INCREMENT;
        DMA_InitStruct.SrcTransferWidth = DMA_TR_WIDTH_8BITS;
        DMA_InitStruct.SrcBurstLength = DMA_BURST_LENGTH_1;
        DMA_InitStruct.SrcLinkedListCtrl = DISABLE;
        DMA_InitStruct.SrcHandshake = DMA_HS_HARDWARE;
        DMA_InitStruct.SrcHWHandshakePolarity = DMA_HS_POLARITY_HIGH;
        DMA_InitStruct.SrcHWHandshakeInterface = 0U;

        DMA_InitStruct.DstAddress = (uint32_t)&(QSPI1->DR[0]);
        DMA_InitStruct.DstMode = DMA_MODE_NORMAL;
        DMA_InitStruct.DstIncMode = DMA_INC_MODE_NO_CHANGE;
        DMA_InitStruct.DstTransferWidth = DMA_TR_WIDTH_8BITS;
        DMA_InitStruct.DstBurstLength = DMA_BURST_LENGTH_1;
        DMA_InitStruct.DstLinkedListCtrl = DISABLE;
        DMA_InitStruct.DstHandshake = DMA_HS_HARDWARE;
        DMA_InitStruct.DstHWHandshakePolarity = DMA_HS_POLARITY_HIGH;
        DMA_InitStruct.DstHWHandshakeInterface = 6U;

        DMA_InitStruct.Direction = DMA_DIR_MEMORY_TO_PERIPH;
        DMA_InitStruct.Priority = DMA_PRIORITY_7;
        DMA_InitStruct.FIFOMode = DMA_FIFO_MODE_0;
        DMA_InitStruct.LinkedListPointer = 0x0UL;
        DMA_InitStruct.NbData = (uint8_t)DataLen;

        uint32_t BlocksNum = (DataLen + DMA_LLI_BLOCK_SIZE - 1U) / DMA_LLI_BLOCK_SIZE;

        if (BlocksNum > DMA_LLI_ARRAY_SIZE)
            BlocksNum = DMA_LLI_ARRAY_SIZE;

        if (BlocksNum > 1UL)
        {
            /* Multi-block transfer */
            uint32_t LastBlockIndex = BlocksNum - 1UL;

            /* Init LLI for all blocks except the last */
            DMA_InitStruct.NbData = DMA_LLI_BLOCK_SIZE;
            DMA_InitStruct.SrcLinkedListCtrl = ENABLE;

            for (uint32_t i = 0UL; i < LastBlockIndex; i++)
            {
                DMA_InitStruct.LinkedListPointer = (uint32_t)&DMA_LLI_Chain[i+1UL];
                DMA_LLI_Init(&DMA_LLI_Chain[i], &DMA_InitStruct);

                DMA_InitStruct.SrcAddress += DMA_LLI_BLOCK_SIZE;
            }

            /* Init LLI for the last block */
            DMA_InitStruct.NbData = DataLen - LastBlockIndex * DMA_LLI_BLOCK_SIZE;
            DMA_InitStruct.SrcLinkedListCtrl = DISABLE;
            DMA_InitStruct.LinkedListPointer = 0x0UL;

            DMA_LLI_Init(&DMA_LLI_Chain[LastBlockIndex], &DMA_InitStruct);

            /* Set LLP for multi-block data transmission */
            DMA_InitStruct.SrcLinkedListCtrl = ENABLE;
            DMA_InitStruct.LinkedListPointer = (uint32_t)&DMA_LLI_Chain[0];
        }
        
        /* Init DMA Tx channel */
        DMA1_Init(DMA_TX_CHANNEL, &DMA_InitStruct);
    }
}

/**
 * @brief Set up DMA Rx transfer.
 * @note The function prepares multi-block data transfer,
 *       if the data requires more than one DMA block.
 * @param DataBuf The data buffer.
 * @param DataLen The data length.
 */
void DMA1_SetupRx(uint8_t* DataBuf, uint32_t DataLen)
{
    if (DataLen > 0UL)
    {
        DMA_InitStruct_TypeDef DMA_InitStruct;
        DMA_StructInit(&DMA_InitStruct);

        DMA_InitStruct.SrcAddress = (uint32_t)&(QSPI1->DR[0]);
        DMA_InitStruct.SrcMode = DMA_MODE_NORMAL;
        DMA_InitStruct.SrcIncMode = DMA_INC_MODE_NO_CHANGE;
        DMA_InitStruct.SrcTransferWidth = DMA_TR_WIDTH_8BITS;
        DMA_InitStruct.SrcBurstLength = DMA_BURST_LENGTH_1;
        DMA_InitStruct.SrcLinkedListCtrl = DISABLE;
        DMA_InitStruct.SrcHandshake = DMA_HS_HARDWARE;
        DMA_InitStruct.SrcHWHandshakePolarity = DMA_HS_POLARITY_HIGH;
        DMA_InitStruct.SrcHWHandshakeInterface = 7U;

        DMA_InitStruct.DstAddress = (uint32_t)DataBuf;
        DMA_InitStruct.DstMode = DMA_MODE_NORMAL;
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
        DMA_InitStruct.NbData = (uint8_t)DataLen;

        uint32_t BlocksNum = (DataLen + DMA_LLI_BLOCK_SIZE - 1U) / DMA_LLI_BLOCK_SIZE;

        if (BlocksNum > DMA_LLI_ARRAY_SIZE)
            BlocksNum = DMA_LLI_ARRAY_SIZE;

        if (BlocksNum > 1UL)
        {
            /* Multi-block transfer */
            uint32_t LastBlockIndex = BlocksNum - 1UL;
            
            /* Init LLI for all blocks except the last */
            DMA_InitStruct.NbData = DMA_LLI_BLOCK_SIZE;
            DMA_InitStruct.DstLinkedListCtrl = ENABLE;

            for (uint32_t i = 0UL; i < LastBlockIndex; i++)
            {
                DMA_InitStruct.LinkedListPointer = (uint32_t)&DMA_LLI_Chain[i+1UL];
                DMA_LLI_Init(&DMA_LLI_Chain[i], &DMA_InitStruct);

                DMA_InitStruct.DstAddress += DMA_LLI_BLOCK_SIZE;
            }

            /* Init LLI for the last block */
            DMA_InitStruct.NbData = DataLen - LastBlockIndex * DMA_LLI_BLOCK_SIZE;
            DMA_InitStruct.DstLinkedListCtrl = DISABLE;
            DMA_InitStruct.LinkedListPointer = 0x0UL;

            DMA_LLI_Init(&DMA_LLI_Chain[LastBlockIndex], &DMA_InitStruct);

            /* Set LLP for multi-block data transmission */
            DMA_InitStruct.DstLinkedListCtrl = ENABLE;
            DMA_InitStruct.LinkedListPointer = (uint32_t)&DMA_LLI_Chain[0];
        }
        
        /* Init DMA Rx channel */
        DMA1_Init(DMA_RX_CHANNEL, &DMA_InitStruct);
    }
}

/**
 * @brief Switch QSPI interface from standard to enhanced mode.
 */
void QSPI1_StdToEnh(void)
{
    QSPI_SetFrameFormat(QSPI1, QSPI_FRAMEFORMAT_QUAD);
}

/** 
 * @brief Perform QSPI data transaction (transmit and receive) in standard mode.
 * @param DataBuf The data buffer.
 * @param DataLen The data length.
 */
void QSPI1_Std_Xfer(uint8_t* DataBuf, uint32_t DataLen)
{
    /* Set up DMA Rx transfer */
    DMA1_SetupRx(DataBuf, DataLen);
    /* Set up DMA Tx transfer */
    DMA1_SetupTx(DataBuf, DataLen);

    QSPI_Enable(QSPI1);

    /* Enable DMA Rx channel */
    DMA_EnableChannel(DMA1, DMA_RX_CHANNEL);
    /* Enable DMA Tx channel */
    DMA_EnableChannel(DMA1, DMA_TX_CHANNEL);

    /* Wait until the Tx FIFO is not empty */
    while (QSPI_IsActiveFlag(QSPI1, QSPI_FLAG_TFE) != 0UL);

    /* Tx data is already in FIFO */
    /* Assert CS to start transfer */
    QSPI_SetSlaveSelect(QSPI1, QSPI_SSEL_0);

    /* Wait for DMA Rx transfer to complete */
    while (DMA_IsActiveIT_IntTfr(DMA1, DMA_RX_CHANNEL) != 1UL);

    /* The data have been received by the DMA channel */
    /* No need to wait for BUSY flag */
    QSPI_SetSlaveSelect(QSPI1, QSPI_SSEL_NOT_SELECTED);

    QSPI_Disable(QSPI1);
}

/** 
 * @brief Send QSPI read command in enhanced mode.
 * @note The command is written directly in FIFO, the data
 *       is received by the DMA.
 * @note For correct operation the data length must not be 0.
 * @param Inst The instruction. It always takes one byte.
 * @param Addr The address (little endian).
 * @param AddrLen The address length (up to four bytes).
 * @param DummyCycles The number of dummy cycles.
 * @param DataBuf The data buffer.
 * @param DataLen The data length.
 */
void QSPI1_Enh_Read(uint8_t Inst,
                    uint32_t Addr,
                    QSPI_AddrLen_TypeDef AddrLen,
                    QSPI_WaitCycles_TypeDef DummyCycles,
                    uint8_t* DataBuf,
                    uint32_t DataLen)
{
    /* Set read mode */
    QSPI_SetTransferMode(QSPI1, QSPI_XFER_RX);

    /* Set address length */
    QSPI_SetAddrLen(QSPI1, AddrLen);

    /* Set wait cycles */
    QSPI_SetWaitCycles(QSPI1, DummyCycles);

    /* Set the number of data bytes */
    QSPI_SetNumOfDataFrames(QSPI1, (uint16_t)(DataLen - 1UL));

    /* Set up DMA Rx transfer for data */
    DMA1_SetupRx(DataBuf, DataLen);

    QSPI_Enable(QSPI1);

    /* Write the instruction */
    QSPI_TransmitData8(QSPI1, Inst);

    /* Write the address */
    /* The address must be written in a single frame */
    if (AddrLen > 0UL)
        QSPI_TransmitData32(QSPI1, Addr);

    /* Enable DMA Rx channel */
    DMA_EnableChannel(DMA1, DMA_RX_CHANNEL);

    /* Tx data is already in FIFO */
    /* Assert CS to start transfer */
    QSPI_SetSlaveSelect(QSPI1, QSPI_SSEL_0);

    /* Wait for DMA Rx transfer to complete */
    while (DMA_IsActiveIT_IntTfr(DMA1, DMA_RX_CHANNEL) != 1UL);

    /* The data have been received by the DMA channel */
    /* No need to wait for BUSY flag */
    QSPI_SetSlaveSelect(QSPI1, QSPI_SSEL_NOT_SELECTED);

    QSPI_Disable(QSPI1);
}

/** 
 * @brief Send QSPI write command in enhanced mode.
 * @note The command is written directly in FIFO, the data
 *       is transmitted by the DMA.
 * @note For correct operation the data length must not be 0.
 * @param Inst The instruction. It always takes one byte.
 * @param Addr The address (little endian).
 * @param AddrLen The address length (up to four bytes).
 * @param DataBuf The data buffer.
 * @param DataLen The data length.
 */
void QSPI1_Enh_Write(uint8_t Inst,
                     uint32_t Addr,
                     QSPI_AddrLen_TypeDef AddrLen,
                     uint8_t* DataBuf,
                     uint32_t DataLen)
{
    /* Set write mode */
    QSPI_SetTransferMode(QSPI1, QSPI_XFER_TX);

    /* Set address length */
    QSPI_SetAddrLen(QSPI1, AddrLen);

    /* Set up DMA Tx transfer for data */
    DMA1_SetupTx(DataBuf, DataLen);

    QSPI_Enable(QSPI1);

    /* Write the instruction */
    QSPI_TransmitData8(QSPI1, Inst);

    /* Write the address */
    /* The address must be written in a single frame */
    if (AddrLen > 0UL)
        QSPI_TransmitData32(QSPI1, Addr);

    /* Enable DMA Tx channel */
    if (DataLen > 0UL)
        DMA_EnableChannel(DMA1, DMA_TX_CHANNEL);

    /* Tx data is already in FIFO */
    /* Assert CS to start transfer */
    QSPI_SetSlaveSelect(QSPI1, QSPI_SSEL_0);

    /* Wait for DMA Tx transfer to complete */
    if (DataLen > 0UL)
        while (DMA_IsActiveIT_IntTfr(DMA1, DMA_TX_CHANNEL) != 1UL);

    /* Wait for QSPI data transfer to complete */
    while (QSPI_IsActiveFlag(QSPI1, QSPI_FLAG_TFE) == 0UL);
    while (QSPI_IsActiveFlag(QSPI1, QSPI_FLAG_BUSY) != 0UL);

    QSPI_SetSlaveSelect(QSPI1, QSPI_SSEL_NOT_SELECTED);

    QSPI_Disable(QSPI1);
}
