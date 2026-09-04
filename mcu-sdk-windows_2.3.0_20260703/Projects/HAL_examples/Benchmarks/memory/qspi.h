/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/Benchmarks/memory/qspi.h
 *  @author     Baikal electronics SDK team
 *  @brief      QSPI example header file
 *  @version    2.3.0
 *  @date       20260703
 *
 *  File contains QSPI interface related functions prototypes
 * *****************************************************************************
 *  @copyright Copyright (c) 2025-2026 Baikal Electronics
 *
 *  This software is licensed under terms that can be found in the LICENSE file
 *  in the root directory of this software component.
 *  If no LICENSE file comes with this software, it is provided AS-IS.
 * *****************************************************************************
 */

#ifndef QSPI_H__
#define QSPI_H__
#include <stdint.h>
#include <stdbool.h>
#include "bmcu_def.h"
#include "bmcu_qspi.h"

/**
 * @brief   Show the status of QSPI interface
 * @param   str not used
 * @retval  none
 */
void qspi_if_show(char *str);

/**
 * @brief   Initialize the QSPI interface with default settings
 * @note    Enable QSPI and corresponding GPIO clock,
 *          set GPIO configuration, set required QSPI clock divider
 * @param   str not used
 * @retval  none
 */
void qspi_if_init(char *str);

/**
 * @brief   Manual enable or disable QSPI interface
 * @note    Not required for most QSPI operations
 * @param   str ASCII string defines the desired QSPI interface state
 *          See cli_is_param_affirmative() for more details
 * @retval  none
 */
void qspi_if_set(char *str);


/**
 * @brief   Set the transfer direction of QSPI interface
 * @param   str ASCII string defines the desired transfer direction:
 *          "tx" - transmit only (standard moe) or write (enhanced mode)
 *          "rx" - receive only (standard moe) or read (enhanced mode)
 *          "eeprom" - EEPROM Read mode
 * @retval  none
 */
void qspi_set_TransferDirection(char *str);
/**
 * @brief   Set the data frame size for QSPI interface
 * @param   str ASCII string defines the data frame size (4..32 bits)
 * @retval  none
 */
void qspi_set_DataFrameSize(char *str);
/**
 * @brief   Enable or disable the endianness for XIP and data register reads
 *          for QSPI interface
 * @param   str ASCII string defines endianness
 *          See cli_is_param_affirmative() for more details
 * @retval  none
 */
void qspi_set_EndiaConversion(char *str);
/**
 * @brief   Set the clock divider value for QSPI interface
 * @param   str ASCII string defines the clock divider value (2..QSPI_SCKDV_MAX)
 * @retval  none
 */
void qspi_set_ClkDivider(char *str);
/**
 * @brief   Set the clock parity value of QSPI interface
 * @param   str ASCII string defines the clock parity value:
 *          "high" - inactive state of serial clock is high
 *          "low"  - inactive state of serial clock is low
 * @retval  none
 */
void qspi_set_ClockPolarity(char *str);
/**
 * @brief   Set the clock phase value of QSPI interface
 * @param   str ASCII string defines the clock phase value
 *          "middle" - serial clock toggles in middle of first data bit
 *          "start"  - serial clock toggles at start of first data bit
 * @retval  none
 */
void qspi_set_ClockPhase(char *str);
/**
 * @brief   Set the SPI data frame format of QSPI interface
 * @param   str ASCII string defines desired SPI data frame format:
 *          "std"  - Standard
 *          "dual" - Dual
 *          "quad" - Quad
 * @retval  none
 */
void qspi_set_FrameFormat(char *str);
/**
 * @brief  Enable of disable slave select toggling of QSPI interface
 * @param   str ASCII string defines the desired slave select toggling state
 *          See cli_is_param_affirmative() for more details
 * @retval  none
 */
void qspi_set_SlaveSelToggle(char *str);
/**
 * @brief   Configure address and instruction transfer mode in SPI mode
 *          of QSPI interface
 * @param   str ASCII string defines the desired mode:
 *          "std"  - instruction and address in standard mode
 *          "inst" - instruction in standard mode, address in "Data frame format" mode
 *          "both" - instruction and address in "Data frame format" mode
 * @retval  none
 */
void qspi_set_TransferMode(char *str);
/**
 * @brief   Set the the length of address to be transmitted in SPI mode of QSPI interface
 * @param   str ASCII string defines the desired address length (0,4,8..60 bits)
 * @retval  none
 */
void qspi_set_AddrLen(char *str);
/**
 * @brief   Set the instruction length in enhanced mode of QSPI interface
 * @param   str ASCII string defines desired instruction length (0,4,8 or 16 bits)
 * @retval  none
 */
void qspi_set_InstLen(char *str);
/**
 * @brief   Set the the number of wait cycles in enhanced mode of QSPI interface
 * @param   str ASCII string defines the number of wait cycles (0..31 bits)
 * @retval  none
 */
void qspi_set_WaitCycles(char *str);
/**
 * @brief   Enable of disable Dual-data rate transfers in enhanced mode of QSPI interface
 * @param   str ASCII string defines the desired mode
 *          See cli_is_param_affirmative() for more details
 * @retval  none
 */
void qspi_set_DDRMode(char *str);
/**
 * @brief   Enable of disable Dual-data rate transfers for Instruction phase in
 *          enhanced mode of QSPI interface
 * @param   str ASCII string defines the desired mode
 *          See cli_is_param_affirmative() for more details
 * @retval  none
 */
void qspi_set_DDRInstMode(char *str);

/*  Transceiv/receive 8-bit data in standard mode; polling
    1. The interface supposed to be disabled before this function call
    2. SPI baud rate, polarity, phase, xfer direction and other common (not the
    current transfer specific settings) should be pre-configured
    3. The transfer is stopped by the shift control logic when the transmit FIFO is empty
    4. It is possible to use the same buffer for Tx and Rx
*/
void qspi_std_txrx_8b(uint8_t *txbuf, uint32_t xfercntr, uint8_t *rxbuf);

/*  Transceiv/receive 8-bit data in standard EEPROM mode; polling
    This mdoe can also be called "command" mode.
    1. The interface supposed to be disabled before this function call
    2. SPI baud rate, polarity, phase, xfer direction and other common (not the
    current transfer specific settings) should be pre-configured
    3. During the transmission of Tx FIFO data, no data is captured by the receive
    logic (as long as the QSPI is transmitting data on its txd line, data on the
    rxd line is ignored). The QSPI continues to transmit data until the transmit
    FIFO is empty. Therefore, you should ONLY have enough data frames in the
    transmit FIFO to supply the control data (command code, etc.).
    If more data frames are in the transmit FIFO than are needed, then read
    data is lost.
    When the transmit FIFO becomes empty (all control information has been sent),
    data on the receive line (rxd) is valid and is stored in the receive FIFO;
    the txd output is held at a constant logic level. The serial transfer
    continues until the number of data frames received by the QSPI matches the
    value of the NDF field in the CTRLR1 register + 1.
*/
void qspi_std_cmd_8b(uint8_t *txbuf, uint32_t txcntr, uint8_t *rxbuf, uint32_t rxcntr);

/*  Receive 8-bit data in standard mode; polling
    This transfer mode increases the bandwidth of the APB bus as the
    transmit FIFO never needs to be serviced during the transfer.
    1. The interface supposed to be disabled before this function call
    2. SPI baud rate, polarity, phase and other common (not the
    current transfer specific settings) should be pre-configured
    3. The end of the serial transfer is controlled by the “number of data
    frames” (NDF) field in control register 1 (CTRLR1; QSPI_FIFO_DEPTH values max).
*/
void qspi_std_rx_8b(uint8_t *buf, uint16_t len);

/*  Transmit 8-bit data in standard mode; polling
    This transfer mode increases the bandwidth of the APB bus as the
    receive FIFO never needs to be serviced during the transfer.
    1. The interface supposed to be disabled before this function call
    2. SPI baud rate, polarity, phase and other common (not the
    current transfer specific settings) should be pre-configured
    3. Transfers are terminated by the shift control logic when the transmit
    FIFO is empty. For continuous data transfers, the transmit FIFO buffer
    must not become empty before all the data have been transmitted.
*/
void qspi_std_tx_8b(uint8_t *buf, uint16_t len);

/*  Read 8-bit data in enhanced mode (Dual/Quad); polling;
    This transfer mode increases the bandwidth of the APB bus as the
    transmit FIFO never needs to be serviced during the transfer.
    1. The interface supposed to be disabled before this function call
    2. SPI baud rate, polarity, phase and other common (not the
    current transfer specific settings) should be pre-configured
    3. The end of the serial transfer is controlled by the “number of data
    frames” (NDF) field in control register 1 (CTRLR1; QSPI_FIFO_DEPTH values max).
    4. Instruction and address will be transmitted before the data receiving phase.
    Instruction and address length must be pre-configured for desired QSPI
    interface and be in accordance with the slave device settings.
*/
void qspi_enh_rd_8b(uint8_t inst, uint32_t addr, uint8_t *buf, uint16_t len);

/*  Write 8-bit data in enhanced mode (Dual/Quad); polling
    This transfer mode increases the bandwidth of the APB bus as the
    receive FIFO never needs to be serviced during the transfer.
    1. The interface supposed to be disabled before this function call
    2. SPI baud rate, polarity, phase, wait cycles and other common (not the
    current transfer specific) settings should be pre-configured
    3. Transfers are terminated by the shift control logic when the transmit
    FIFO is empty. For continuous data transfers, the transmit FIFO buffer
    must not become empty before all the data have been transmitted.
    4. Instruction and address will be transmitted before the data transmitting phase.
    Instruction and address length must be pre-configured for desired QSPI
    interface and be in accordance with the slave device settings.
*/
void qspi_enh_wr_8b(uint8_t inst, uint32_t addr, uint8_t *buf, uint16_t len);

/*  Enables the QSPI master to behave as a memory mapped
    I/O and fetches the data from the device based on the APB read request.
    The eXecute In Place (XIP) mode enables transfer of SPI data directly
    through the APB interface without writing the data register of QSPI.
    XIP mode can be enabled in QSPI by selecting the QSPI_XIP_EN configuration
    parameter, which includes an extra sideband signal xip_en, set by the
    CRU block, on the APB interface.
    This signal indicates whether APB transfers are register read-write or
    XIP reads. If the xip_en signal is driven to 1, QSPI expects only read
    request on the APB interface. This request is translated to SPI read on the
    serial interface and soon after the data is received, the data is returned
    to the APB interface in the same transaction.
    The XIP operation is supported only in enhanced SPI modes (Dual and Quad).
*/
void qspi_xip_enable(void);
void qspi_xip_disable(void);
bool qspi_xip_isenabled(void);
#endif // QSPI_H__
