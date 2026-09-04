/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/USB/Device/USB_DFU/qspi.c
 *  @author     Baikal electronics SDK team
 *  @brief      MT25QL128 QSPI flash memory access driver
 *  @version    2.3.0
 *  @date       20260703
 *
 *  File content:
 *      - (Q)SPI transfer functions
 *          qspi_std_txrx_8b()
 *          qspi_std_cmd_8b()
 *          qspi_enh_rd_8b()
 *          qspi_enh_wr_8b()
 *      - Flash memory access functions
 *          qspi_init()
 *          qspi_flash_read()
 *          qspi_flash_write()
 *          qspi_flash_erase()
 *          qspi_flash_reg_read()
 *          qspi_flash_set_write()
 *          qspi_flash_wait_write()
 *          qspi_flash_set_quad()
 * *****************************************************************************
 *  @copyright Copyright (c) 2025 Baikal Electronics JSC
 *
 *  This software is licensed under terms that can be found in the LICENSE file
 *  in the root directory of this software component.
 *  If no LICENSE file comes with this software, it is provided AS-IS.
 * *****************************************************************************
 */

#include <stdbool.h>
#include <stdio.h>
#include "bmcu_cru.h"
#include "mt25ql128aba.h"
#include "qspi.h"

#define QSPI_FREQUENCY (1 * 1000 * 1000) // 1 Mhz
#define REG_READ_RETRY_CNT  (10U)
#define REG_READ_RETRY_INTERVAL  (100U)

static struct {
    bool std_mode;      ///< Standard or Quad mode
} state = {
    .std_mode = true,
};


// Data transfer functions -----------------------------------------------------
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


static void
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


static void
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


static void
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


static void
qspi_enh_wr_8b (uint8_t inst, uint32_t addr, const uint8_t *buf, uint16_t len) {
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
// -----------------------------------------------------------------------------

static void
qspi_flash_set_write (void) {
    uint8_t cmd = SPI_FLASH_INS_WREN;

    if (state.std_mode) {
        qspi_std_txrx_8b(&cmd, 1, &cmd);

    } else {
        qspi_enh_wr_8b(cmd, 0, &cmd, 0);
    }
}


static uint16_t
qspi_flash_reg_read (uint8_t reg_read_cmd) {
    uint16_t retval = 0;
    uint8_t buf[2] = {reg_read_cmd, 0};

    if (state.std_mode) {
        qspi_std_cmd_8b(buf, 1, buf, (SPI_FLASH_INS_RDNVCR == reg_read_cmd) ? 2 : 1);

    } else {
        qspi_enh_rd_8b(buf[0], 0, buf, (SPI_FLASH_INS_RDNVCR == reg_read_cmd) ? 2 : 1);
    }

    retval = (SPI_FLASH_INS_RDNVCR == reg_read_cmd) ? ((buf[1] << 8) | buf[0]) : buf[0];

    return retval;
}


/**
 * @brief   Wait for the write in progress bit
 * @note    When the operation is in progress, the program or erase controller bit
 *          of the flag status register is set to 0. In addition, the write in progress
 *          bit is set to 1. When the operation completes, the write in progress bit
 *          is cleared to 0.
 * @retval  0 if write in progress bit was erased without timeout, else 1
 */
static int
qspi_flash_wait_write (void) {
    int retval = 0;
    uint32_t try_cnt = REG_READ_RETRY_CNT;
    uint8_t statreg = qspi_flash_reg_read(SPI_FLASH_INS_RDSR);

    while ((1 == (REG_STATUS_WRITEINPROGRESS_MSK & statreg) && try_cnt--)) {
        statreg = qspi_flash_reg_read(SPI_FLASH_INS_RDSR);
        __delay_ms(REG_READ_RETRY_INTERVAL);
    }

    if (0 == try_cnt) {
        retval = 1;
    }

    return retval;
}


void
qspi_flash_erase (void) {
    uint8_t cmd = SPI_FLASH_INS_BE;

    printf("Flash memory bulk erase: ");
    qspi_flash_set_write();

    if (state.std_mode) {
        qspi_std_txrx_8b(&cmd, 1, &cmd);

    } else {
        qspi_enh_wr_8b(cmd, 0, &cmd, 0);
    }

    printf("%s\r\n", (0 == qspi_flash_wait_write()) ? "done" : "error");
}


void
qspi_flash_set_quad (FunctionalState newstate) {
    uint8_t buf[2];
    uint8_t regval;

    printf("Quad mode ");

    if (ENABLE == newstate) {    // Enable Quad mode
        printf("enable\r\n");

        if (!state.std_mode) {
            printf("Already in Quad mode\r\n");
            return;
        }

        // SPI flash memory: get the current state of Enhanced Volatile Configuration Register
        regval = qspi_flash_reg_read(SPI_FLASH_INS_RDVECR);
        // Enable quad I/O command input (4-4-4 mode)
        regval = regval & ~REG_VEC_QUAD_MSK;
        // Write back the Enhanced Volatile Configuration Register
        buf[0] = SPI_FLASH_INS_WRVECR;
        buf[1] = regval;
        qspi_flash_set_write();
        qspi_std_txrx_8b(buf, 2, buf);

        // SPI flash memory: enter Quad I/O mode
        buf[0] = SPI_FLASH_INS_QUAD_ENTER;
        qspi_std_txrx_8b(buf, 1, buf);

        // QSPI interface: switch to the Quad mode
        QSPI_SetFrameFormat(QSPI1, QSPI_FRAMEFORMAT_QUAD);
        QSPI_SetAddrInstXferMode(QSPI1, QSPI_XFER_BOTH);

        state.std_mode = false;

    } else {    // Disable Quad mode
        printf("disable\r\n");

        if (state.std_mode) {
            return;
        }

        // SPI flash memory: get the current state of Enhanced Volatile Configuration Register
        regval = qspi_flash_reg_read(SPI_FLASH_INS_RDVECR);
        // Disable quad I/O command input (4-4-4 mode)
        regval = regval | REG_VEC_QUAD_MSK;
        // Write back the Enhanced Volatile Configuration Register
        qspi_flash_set_write();
        qspi_enh_wr_8b(SPI_FLASH_INS_WRVECR, 0, &regval, 1);

        // SPI flash memory: reset Quad I/O mode
        qspi_enh_wr_8b(SPI_FLASH_INS_QUAD_RESET, 0, &regval, 0);

        // QSPI interface: switch to the standard mode
        QSPI_SetFrameFormat(QSPI1, QSPI_FRAMEFORMAT_STD);
        QSPI_SetAddrInstXferMode(QSPI1, QSPI_XFER_STD);

        state.std_mode = true;
    }
}


void
qspi_flash_read (uint32_t addr, uint8_t *data, uint16_t size) {
    QSPI_AddrLen_TypeDef prev_adlen;
    QSPI_WaitCycles_TypeDef prev_wcycles;
    uint8_t xfer_ctrl[4];

    if (state.std_mode) {
        xfer_ctrl[0] = SPI_FLASH_INS_READ;
        xfer_ctrl[1] = (addr >> 16) & 0xFF;
        xfer_ctrl[2] = (addr >> 8) & 0xFF;
        xfer_ctrl[3] = addr & 0xFF;
        qspi_std_cmd_8b(xfer_ctrl, 4, data, size);

    } else {
        prev_adlen = QSPI_GetAddrLen(QSPI1);
        prev_wcycles = QSPI_GetWaitCycles(QSPI1);
        QSPI_SetAddrLen(QSPI1, QSPI_ADDRLEN_24);
        QSPI_SetWaitCycles(QSPI1, QSPI_WAITCYCLES_10);
        qspi_enh_rd_8b(SPI_FLASH_INS_QIOFR, addr, data, size);
        QSPI_SetAddrLen(QSPI1, prev_adlen);
        QSPI_SetWaitCycles(QSPI1, prev_wcycles);
    }

}


/// Quad mode only
void
qspi_flash_write (uint32_t addr, const uint8_t *data, uint16_t size) {
    uint16_t chunk_size;

    while (size > 0) {
        chunk_size = SPI_FLASH_PAGE_SIZE - addr % SPI_FLASH_PAGE_SIZE;

        if (size < chunk_size) {
            chunk_size = size;
        }

        QSPI_SetAddrLen(QSPI1, QSPI_ADDRLEN_0);
        qspi_flash_set_write();
        QSPI_SetAddrLen(QSPI1, QSPI_ADDRLEN_24);
        qspi_enh_wr_8b(SPI_FLASH_INS_QIFP, addr, data, chunk_size);

        addr += chunk_size;
        data += chunk_size;
        size -= chunk_size;
    }
}


void
qspi_init (void) {
    QSPI_InitStruct_TypeDef QSPI_InitStruct;
    CRU_Clocks_TypeDef CRU_Clocks;
    uint32_t freq;

    CRU_APB1_EnableClock(CRU_APB1_PERIPH_GPIO1);
    CRU_APB1_EnableClock(CRU_APB1_PERIPH_QSPI1);

    printf("Set GPIO mode to AF3 (QSPI mode)\r\n");
    CRU_SetPinAF(CRU_PORT_B, CRU_PIN_0, CRU_PIN_AF_3);      // QSPI1_CSN
    CRU_SetPinAF(CRU_PORT_B, CRU_PIN_1, CRU_PIN_AF_3);      // QSPI1_SCK
    CRU_SetPinAF(CRU_PORT_B, CRU_PIN_2, CRU_PIN_AF_3);      // QSPI1_IO0
    CRU_SetPinAF(CRU_PORT_B, CRU_PIN_3, CRU_PIN_AF_3);      // QSPI1_IO1
    CRU_SetPinAF(CRU_PORT_B, CRU_PIN_4, CRU_PIN_AF_3);      // QSPI1_IO2
    CRU_SetPinAF(CRU_PORT_B, CRU_PIN_5, CRU_PIN_AF_3);      // QSPI1_IO3

    /* GPIO driver strength control is required for QSPI high speed */
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

    printf("QSPI interface init\r\n");
    QSPI_Disable(QSPI1);
    QSPI_SetSlaveSelect(QSPI1, QSPI_SSEL_NOT_SELECTED);
    QSPI_StructInit(&QSPI_InitStruct);
    CRU_GetSystemClocksFreq(&CRU_Clocks);
    freq = CRU_Clocks.PCLK1_Frequency;
    QSPI_InitStruct.ClkDivider = (freq / QSPI_FREQUENCY) & QSPI_SCKDV_MAX;
    QSPI_Init(QSPI1, &QSPI_InitStruct);
}
