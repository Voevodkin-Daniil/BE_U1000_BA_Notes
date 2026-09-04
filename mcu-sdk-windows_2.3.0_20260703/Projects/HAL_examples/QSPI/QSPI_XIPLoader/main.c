/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/QSPI/QSPI_XIPLoader/main.c
 *  @author     Baikal electronics SDK team
 *  @brief      XIP Loader example
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

#include <stdbool.h>
#include "bmcu_cru.h"
#include "bmcu_eflash.h"
#include "bmcu_qspi.h"
#include "mt25ql128aba.h"

#ifndef CLK_CCLK
#define CLK_CCLK                200
#endif

#ifndef QSPI_CLK
#define QSPI_CLK                25
#endif

#define QSPI_FREQUENCY          (QSPI_CLK  * 1000 * 1000)
#define REG_READ_RETRY_CNT      (10U)
#define REG_READ_RETRY_INTERVAL (100U)


/**
 * @brief   CPU clock setup (PLL, Core, APB[0..2], AHB)
 */
void
CLK_Init (void) {
    CRU_PLL_BypassMode_Disable();

    /// Set PLL source clock to C0
    CRU_PLL_SetSource(CRU_PLL_SRC_C0);

    /// PLL: 200 MHz
    CRU_PLL_ForceReset();
    CRU_PLL_Config(1UL, 48UL, 6UL, 48UL);
    CRU_PLL_ReleaseReset();

    /// PLL lock time min. 500 cycles
    __delay_cycles(500UL);

    /* Configure CCLK */
#if CLK_CCLK == 200
    CRU_SetCCLKPrescaler(CRU_CLK_DIV_1);
#elif CLK_CCLK == 100
    CRU_SetCCLKPrescaler(CRU_CLK_DIV_2);
#elif CLK_CCLK == 25
    CRU_SetCCLKPrescaler(CRU_CLK_DIV_8);
#else
    #error CLK_CCLK is invalid or not set
#endif
    CRU_CCLKPrescaler_Enable();
    CRU_SetCCLKSource(CRU_CLK_SRC_PLL);

    /// PCLK0: 100 MHz
    CRU_SetPCLK0Prescaler(CRU_CLK_DIV_2);
    CRU_PCLK0Prescaler_Enable();
    CRU_SetPCLK0Source(CRU_CLK_SRC_PLL);

    /// PCLK1: 100 MHz
    CRU_SetPCLK1Prescaler(CRU_CLK_DIV_2);
    CRU_PCLK1Prescaler_Enable();
    CRU_SetPCLK1Source(CRU_CLK_SRC_PLL);

    /// PCLK2: 100 MHz
    CRU_SetPCLK2Prescaler(CRU_CLK_DIV_2);
    CRU_PCLK2Prescaler_Enable();
    CRU_SetPCLK2Source(CRU_CLK_SRC_PLL);

    /// HCLK: 100 MHz
    CRU_SetHCLKPrescaler(CRU_CLK_DIV_2);
    CRU_HCLKPrescaler_Enable();
    CRU_SetHCLKSource(CRU_CLK_SRC_PLL);

    /// 1MHz clocks
#if CLK_CCLK == 200
    CRU_SetCCLK1MHzPrescaler(199UL);
#elif CLK_CCLK == 100
    CRU_SetCCLK1MHzPrescaler(99UL);
#elif CLK_CCLK == 25
    CRU_SetCCLK1MHzPrescaler(24UL);
#else
    #error CLK_CCLK is invalid or not set
#endif

    CRU_SetHCLK1MHzPrescaler(99UL);
}


void
qspi_init (void) {
    QSPI_InitStruct_TypeDef QSPI_InitStruct;
    CRU_Clocks_TypeDef CRU_Clocks;

    CRU_APB1_EnableClock(CRU_APB1_PERIPH_GPIO1);
    CRU_APB1_EnableClock(CRU_APB1_PERIPH_QSPI1);

    CRU_SetPinAF(CRU_PORT_B, CRU_PIN_0, CRU_PIN_AF_3);      ///< QSPI1_CSN
    CRU_SetPinAF(CRU_PORT_B, CRU_PIN_1, CRU_PIN_AF_3);      ///< QSPI1_SCK
    CRU_SetPinAF(CRU_PORT_B, CRU_PIN_2, CRU_PIN_AF_3);      ///< QSPI1_IO0
    CRU_SetPinAF(CRU_PORT_B, CRU_PIN_3, CRU_PIN_AF_3);      ///< QSPI1_IO1
    CRU_SetPinAF(CRU_PORT_B, CRU_PIN_4, CRU_PIN_AF_3);      ///< QSPI1_IO2
    CRU_SetPinAF(CRU_PORT_B, CRU_PIN_5, CRU_PIN_AF_3);      ///< QSPI1_IO3

    /** GPIO driver strength control is required for QSPI high speed */
    CRU_SetPinDriveStrength(CRU_PORT_B, CRU_PIN_0, CRU_PIN_DRIVE_STRENGTH_3);
    CRU_SetPinDriveStrength(CRU_PORT_B, CRU_PIN_1, CRU_PIN_DRIVE_STRENGTH_3);
    CRU_SetPinDriveStrength(CRU_PORT_B, CRU_PIN_2, CRU_PIN_DRIVE_STRENGTH_3);
    CRU_SetPinDriveStrength(CRU_PORT_B, CRU_PIN_3, CRU_PIN_DRIVE_STRENGTH_3);
    CRU_SetPinDriveStrength(CRU_PORT_B, CRU_PIN_4, CRU_PIN_DRIVE_STRENGTH_3);
    CRU_SetPinDriveStrength(CRU_PORT_B, CRU_PIN_5, CRU_PIN_DRIVE_STRENGTH_3);

    CRU_SetPinPull(CRU_PORT_B, CRU_PIN_0, CRU_PIN_PULL_UP);
    CRU_SetPinPull(CRU_PORT_B, CRU_PIN_1, CRU_PIN_PULL_UP);
    CRU_SetPinPull(CRU_PORT_B, CRU_PIN_2, CRU_PIN_PULL_UP);
    CRU_SetPinPull(CRU_PORT_B, CRU_PIN_3, CRU_PIN_PULL_UP);
    CRU_SetPinPull(CRU_PORT_B, CRU_PIN_4, CRU_PIN_PULL_UP);
    CRU_SetPinPull(CRU_PORT_B, CRU_PIN_5, CRU_PIN_PULL_UP);

    /** Input buffer enabling is required for data receiving in the Quad SPI mode */
    CRU_EnablePinInput(CRU_PORT_B, CRU_PIN_2);
    CRU_EnablePinInput(CRU_PORT_B, CRU_PIN_3);
    CRU_EnablePinInput(CRU_PORT_B, CRU_PIN_4);
    CRU_EnablePinInput(CRU_PORT_B, CRU_PIN_5);

    QSPI_Disable(QSPI1);
    QSPI_SetSlaveSelect(QSPI1, QSPI_SSEL_NOT_SELECTED);
    QSPI_StructInit(&QSPI_InitStruct);
    CRU_GetSystemClocksFreq(&CRU_Clocks);
    QSPI_InitStruct.ClkDivider = (CRU_Clocks.PCLK1_Frequency / QSPI_FREQUENCY) & QSPI_SCKDV_MAX;
    QSPI_Init(QSPI1, &QSPI_InitStruct);
}


// QSPI and Flash related functions --------------------------------------------
static struct {
    bool std_mode;      ///< Standard or Quad mode
} state = {
    .std_mode = true,
};


/**
 *  The BUSY status is not set when the data are written into the transmit FIFO
 *  This bit gets set only when the target slave has been selected and the
 *  transfer is underway. After writing data into the transmit FIFO, the shift
 *  logic does not begin the serial transfer until a positive edge of the
 *  sclk_out signal is present. The delay in waiting for this positive edge
 *  depends on the baud rate of the serial transfer.
 *  Before polling the BUSY status, you should first poll the TFE status
 *  (waiting for 1) or wait for BAUDR * qspi_clk clock cycles.
 */
static inline void
qspi_wait_idle (void) {
    while (!QSPI_IsActiveFlag(QSPI1, QSPI_FLAG_TFE));
    while (QSPI_IsActiveFlag(QSPI1, QSPI_FLAG_BUSY));
}


void
qspi_std_txrx_8b (uint8_t *txbuf, uint32_t txcntr, uint8_t *rxbuf) {
    /// For TxRx mode Rx-ed data counter == Tx-ed data counter
    uint32_t rxcntr = txcntr;

    /// Set transfer mode (i.e. data direction processing) to Tx and Rx
    QSPI_SetTransferMode(QSPI1, QSPI_XFER_TXRX);

    /// Set data frame size 8 bits
    QSPI_SetDataFrameSize(QSPI1, QSPI_DATA_FRAME_SIZE_8);

    /// Enable the interface. Transfer will not begin while slave select is released
    QSPI_Enable(QSPI1);

    /// Prefill the Tx FIFO
    while (txcntr && QSPI_IsActiveFlag(QSPI1, QSPI_FLAG_TFNF)) {
        QSPI_TransmitData8(QSPI1, *txbuf++);
        txcntr--;
    }

    /// Set slave select signal (transfer begins at this moment)
    QSPI_SetSlaveSelect(QSPI1, QSPI_SSEL_0);

    /// Write data to Tx FIFO and read data from Rx FIFO
    while (txcntr) {
        if (QSPI_IsActiveFlag(QSPI1, QSPI_FLAG_RFNE)) {
            *rxbuf++ = QSPI_ReceiveData8(QSPI1);
            rxcntr--;
        }

        if (txcntr && QSPI_IsActiveFlag(QSPI1, QSPI_FLAG_TFNF)) {
            QSPI_TransmitData8(QSPI1, *txbuf++);
            txcntr--;
        }
    }

    /// Receive the rest of the data
    while (rxcntr) {
        if (QSPI_IsActiveFlag(QSPI1, QSPI_FLAG_RFNE)) {
            *rxbuf++ = QSPI_ReceiveData8(QSPI1);
            rxcntr--;
        }
    }

    /// Wait for the transfer to complete
    qspi_wait_idle();

    /// Release slave select and disable the interface
    QSPI_SetSlaveSelect(QSPI1, QSPI_SSEL_NOT_SELECTED);
    QSPI_Disable(QSPI1);
}


void
qspi_std_cmd_8b (uint8_t *txbuf, uint32_t txcntr, uint8_t *rxbuf, uint32_t rxcntr) {
    /// Set transfer mode (i.e. data direction processing) to Tx and Rx
    QSPI_SetTransferMode(QSPI1, QSPI_XFER_EEPROM);

    /// Set data frame size 8 bits
    QSPI_SetDataFrameSize(QSPI1, QSPI_DATA_FRAME_SIZE_8);

    /// Set the number of data frames
    QSPI_SetNumOfDataFrames(QSPI1, rxcntr - 1);

    /// Enable the interface. Transfer will not begin while slave select is released
    QSPI_Enable(QSPI1);

    /// Prefill the Tx FIFO
    while (txcntr && QSPI_IsActiveFlag(QSPI1, QSPI_FLAG_TFNF)) {
        QSPI_TransmitData8(QSPI1, *txbuf++);
        txcntr--;
    }

    /// Set slave select signal (transfer begins at this moment)
    QSPI_SetSlaveSelect(QSPI1, QSPI_SSEL_0);

    /// Write data to Tx FIFO. Received data is invalid yet
    while (txcntr) {
        if (QSPI_IsActiveFlag(QSPI1, QSPI_FLAG_TFNF)) {
            QSPI_TransmitData8(QSPI1, *txbuf++);
            txcntr--;
        }
    }

    /*  Tx FIFO becomes empty, data on the receive line
        (rxd) is valid and is stored in the receive FIFO
    */
    while (!QSPI_IsActiveFlag(QSPI1, QSPI_FLAG_TFE));

    /// Receive the rest of the data
    do {
        if (QSPI_IsActiveFlag(QSPI1, QSPI_FLAG_RFNE)) {
            *rxbuf++ = QSPI_ReceiveData8(QSPI1);
        }
    } while (QSPI_IsActiveFlag(QSPI1, QSPI_FLAG_BUSY));

    /// No need to call qspi_wait_idle(): TFE and BUSY flash already checked

    /// Release slave select and disable the interface
    QSPI_SetSlaveSelect(QSPI1, QSPI_SSEL_NOT_SELECTED);
    QSPI_Disable(QSPI1);
}


void
qspi_enh_rd_8b (uint8_t inst, uint32_t addr, uint8_t *buf, uint16_t len) {
    /// Set transfer mode (i.e. data direction processing) to Rx
    QSPI_SetTransferMode(QSPI1, QSPI_XFER_RX);

    /// Set data frame size 8 bits
    QSPI_SetDataFrameSize(QSPI1, QSPI_DATA_FRAME_SIZE_8);

    /// Set the number of data frames
    QSPI_SetNumOfDataFrames(QSPI1, (0 == len) ? 1 : len - 1);

    /// Enable the interface. Transfer will not begin while slave select is released
    QSPI_Enable(QSPI1);

    /// Transfer starts only when instruction and address data is written to the Tx FIFO
    if (QSPI_INSTLEN_0 != QSPI_GetInstLen(QSPI1)) {
        QSPI_TransmitData8(QSPI1, inst);
    }

    if (QSPI_ADDRLEN_0 != QSPI_GetAddrLen(QSPI1)) {
        QSPI_TransmitData32(QSPI1, addr);
    }

    /// Set slave select signal
    QSPI_SetSlaveSelect(QSPI1, QSPI_SSEL_0);

    /// Wait for the control data to be sent
    while (!QSPI_IsActiveFlag(QSPI1, QSPI_FLAG_TFE));

    /// Data receive
    do {
        while (QSPI_IsActiveFlag(QSPI1, QSPI_FLAG_RFNE)) {
            *buf++ = QSPI_ReceiveData8(QSPI1);
        }
    } while (QSPI_IsActiveFlag(QSPI1, QSPI_FLAG_BUSY));

    /// No need to call qspi_wait_idle(): TFE and BUSY flash already checked

    /// Release slave select and disable the interface
    QSPI_SetSlaveSelect(QSPI1, QSPI_SSEL_NOT_SELECTED);
    QSPI_Disable(QSPI1);
}


void
qspi_enh_wr_8b (uint8_t inst, uint32_t addr, uint8_t *buf, uint16_t len) {
    /// Set transfer mode (i.e. data direction processing) to Rx
    QSPI_SetTransferMode(QSPI1, QSPI_XFER_TX);

    /// Set data frame size 8 bits
    QSPI_SetDataFrameSize(QSPI1, QSPI_DATA_FRAME_SIZE_8);

    /// Enable the interface. Transfer will not begin while slave select is released
    QSPI_Enable(QSPI1);

    /// Write instruction and address data to the Tx FIFO
    if (QSPI_INSTLEN_0 != QSPI_GetInstLen(QSPI1)) {
        QSPI_TransmitData8(QSPI1, inst);
    }

    if (QSPI_ADDRLEN_0 != QSPI_GetAddrLen(QSPI1)) {
        QSPI_TransmitData32(QSPI1, addr);
    }

    /// Prefill the Tx FIFO
    while (len && QSPI_IsActiveFlag(QSPI1, QSPI_FLAG_TFNF)) {
        QSPI_TransmitData8(QSPI1, *buf++);
        len--;
    }

    /// Set slave select signal (transfer begins at this moment)
    QSPI_SetSlaveSelect(QSPI1, QSPI_SSEL_0);

    /// Write data to Tx FIFO and read data from Rx FIFO
    while (len) {
        if (QSPI_IsActiveFlag(QSPI1, QSPI_FLAG_TFNF)) {
            QSPI_TransmitData8(QSPI1, *buf++);
            len--;
        }
    }

    /// Wait for the transfer to complete
    qspi_wait_idle();

    /// Release slave select and disable the interface
    QSPI_SetSlaveSelect(QSPI1, QSPI_SSEL_NOT_SELECTED);
    QSPI_Disable(QSPI1);
}


static void
flash_write_en (void) {
    uint8_t cmd = SPI_FLASH_INS_WREN;

    if (state.std_mode) {
        qspi_std_txrx_8b(&cmd, 1, &cmd);

    } else {
        qspi_enh_wr_8b(cmd, 0, &cmd, 0);
    }
}


static uint8_t
flash_reg_read (uint8_t reg_read) {
    uint8_t buf[2] = {reg_read, 0};

    if (state.std_mode) {
        qspi_std_cmd_8b(buf, 1, buf, 1);

    } else {
        qspi_enh_rd_8b(buf[0], 0, buf, 1);
    }

    return buf[0];
}


static int
flash_wait_write (void) {
    int retval = 0;
    uint32_t try_cnt = REG_READ_RETRY_CNT;
    uint8_t statreg = flash_reg_read(SPI_FLASH_INS_RDSR);

    while ((1 == (REG_STATUS_WRITEINPROGRESS_MSK & statreg) && try_cnt--)) {
        statreg = flash_reg_read(SPI_FLASH_INS_RDSR);
        __delay_ms(REG_READ_RETRY_INTERVAL);
    }

    if (0 == try_cnt) {
        retval = 1;
    }

    return retval;
}


void
xip_enable (void) {
    uint8_t regval;
    uint8_t buf[2];
    QSPI_WaitCycles_TypeDef waitcycles = QSPI_WAITCYCLES_14;

    /// 1. Switch the SPI flash IC in Quad SPI mode
    /// SPI flash memory: get the current state of Enhanced Volatile Configuration Register
    regval = flash_reg_read(SPI_FLASH_INS_RDVECR);
    /// Enable quad I/O command input (4-4-4 mode)
    regval = regval & ~REG_VEC_QUAD_MSK;
    /// Write back the Enhanced Volatile Configuration Register
    buf[0] = SPI_FLASH_INS_WRVECR;
    buf[1] = regval;
    flash_write_en();
    qspi_std_txrx_8b(buf, 2, buf);
    /// SPI flash memory: enter Quad I/O mode
    buf[0] = SPI_FLASH_INS_QUAD_ENTER;
    qspi_std_txrx_8b(buf, 1, buf);

    /// 2. Switch the MCU QSPI interface in Quad mode
    QSPI_SetFrameFormat(QSPI1, QSPI_FRAMEFORMAT_QUAD);
    QSPI_SetAddrInstXferMode(QSPI1, QSPI_XFER_BOTH);
    state.std_mode = false;

    /// 3. Switch SPI flash IC in XIP mode
    /// Get the Volatile Configuration Register current state
    regval = flash_reg_read(SPI_FLASH_INS_RDVCR);
    /// Configure SPI flash dummy cycles for FAST READ commands
    regval &= ~REG_VC_DCL_MSK;
    regval |= ((waitcycles >> QSPI_SPI_CTRLR0_WAIT_CYCLES_Pos) << REG_VC_DCL_POS);
    /// Clear XIP mode bit (i.e. enable XIP mode)
    regval &= ~REG_VC_XIP_MSK;
    /// Write back the Volatile Configuration Register
    flash_write_en();
    qspi_enh_wr_8b(SPI_FLASH_INS_WRVCR, 0, &regval, 1);
    flash_wait_write();

    /// Config QSPI interface waitcycles according to SPI flash settings
    QSPI_SetWaitCycles(QSPI1, waitcycles);

    /// Drive the XIP confirmation bit to 0 during FAST READ operation
    QSPI_SetInstLen(QSPI1, QSPI_INSTLEN_8);
    QSPI_SetAddrLen(QSPI1, QSPI_ADDRLEN_28);
    qspi_enh_rd_8b(SPI_FLASH_INS_FAST_READ, 0xFFFFFFE, buf, 0);

    /// 4. Switch MCU QSPI interface in XIP mode
    QSPI_SetClockPolarity(QSPI1, QSPI_POLARITY_LOW);
    QSPI_SetClockPhase(QSPI1, QSPI_PHASE_MIDDLE);

    /// Only read operations allowed in the XIP mode
    QSPI_SetTransferMode(QSPI1, QSPI_XFER_RX);

    /// 32-bit data width fot the XIP reads
    QSPI_SetDataFrameSize(QSPI1, QSPI_DATA_FRAME_SIZE_32);

    QSPI_SetNumOfDataFrames(QSPI1, 0);
    QSPI_EnableEndianConversion(QSPI1);
    QSPI_EnableSlaveSelToggle(QSPI1);
    QSPI_SetFrameFormat(QSPI1, QSPI_FRAMEFORMAT_QUAD);
    QSPI_SetAddrInstXferMode(QSPI1, QSPI_XFER_BOTH);
    QSPI_SetInstLen(QSPI1, QSPI_INSTLEN_0);

    /// 16 MB max
    QSPI_SetAddrLen(QSPI1, QSPI_ADDRLEN_24);

    QSPI_DisableDDR(QSPI1);
    QSPI_DisableInstDDR(QSPI1);

    QSPI_Enable(QSPI1);
    QSPI_SetSlaveSelect(QSPI1, QSPI_SSEL_0);

    /// Enable xip_en sideband signal
    CRU_QSPI1_XIP_Enable();
}


void
xip_exec (void) {
    void(*app)(void) = (void(*)(void))(XIP1_BASE + __QSPI_APP_ADDR__);
    app();
}


// -----------------------------------------------------------------------------
int
main (void) {
    /* Prepare eFlash to CCLK clock. See PLL configuration */
    EFLASH_Init(CLK_CCLK * 1000 * 1000);
    CLK_Init();
    qspi_init();

    /* Set RX_SAMPLE_DLY=1 */
    QSPI_SetRxSampleDelay(QSPI1, 1);

    xip_enable();
    xip_exec();

    return 0;
}
