/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/Benchmarks/memory/qspi_flash.h
 *  @author     Baikal electronics SDK team
 *  @brief      QSPI flash IC example header file
 *  @version    2.3.0
 *  @date       20260703
 * 
 *  File contains QSPI flash memory IC related functions prototypes
 * *****************************************************************************
 *  @copyright Copyright (c) 2025-2026 Baikal Electronics
 *
 *  This software is licensed under terms that can be found in the LICENSE file
 *  in the root directory of this software component.
 *  If no LICENSE file comes with this software, it is provided AS-IS.
 * *****************************************************************************
 */

 #ifndef QSPI_FLASH_H__
#define QSPI_FLASH_H__

/**
 * @brief   Read the ID value of the SPI flash IC
 */
void flash_get_id_cmd(char *str);

/**
 * @brief   To reset the device, the RESET ENABLE command must be followed by the
 *          RESET MEMORY command. When the two commands are executed, the device enters
 *          a power-on reset condition. It is recommended to exit XIP mode before
 *          executing these two commands.
 *          All volatile lock bits, the volatile configuration register, and the
 *          enhanced volatile configuration register are reset to the power-on reset
 *          default condition according to nonvolatile configuration register settings.
*/
void flash_reset_cmd(char *str);

/**
 * @brief   Flash memory bulk erase
 *          Sets the device bits to 0xFF
 */
void flash_erase_cmd(char *str);

/**
 * @brief   Clear the flash status register
 *          Resets the error bits (erase, program, and protection)
 */
void flash_clear_flagsr_cmd(char *str);

/**
 * @brief   Read service registers of the SPI flash
 *          - Status Register
 *          - Flag status register
 *          - Volatile configuration register
 *          - Enhanced volatile configuration register
 */
void flash_get_status_cmd(char *str);

/**
 * @brief   Read flash memory region
 *          No address and size params validation
 *          Allows reading the flash memory in standard, quad and XIP modes
 *          In XIP mode use address value multiples of 4
 * @param   str defines the reading start address and memory region size
 *          to be read
*/
void flash_read_cmd(char *str);

/**
 * @brief   Write data to the flash memory
 *          One page write only
 * @param   str defines the writing start address and the test string 
 *          to be written
 *          Default address (0x00) and data (some ASCII text) will be
 *          written if no parameters
 */
void flash_write_cmd(char *str);

/**
 * @brief   Enable/disable Quad mode of the SPI flash IC
 * @param   str defines the Quad mode to be enabled or disabled
 */
void flash_set_quad_cmd(char *str);

/*  Activate and Terminate XIP Using Volatile Configuration Register
    XIP provides faster memory READ operations by requiring only an address
    to execute, rather than a command code and an address.

    SPI memory requires two steps to activate XIP mode.
    First, enable XIP by setting volatile configuration register bit 3 to 0.
    Next, drive the XIP confirmation bit to 0 during the next FAST READ
    operation. XIP is then active.
*/
void flash_set_xip_cmd(char *str);

/**
 * @brief   Execute the code from the SPI flash in XIP mode
 * @param   str defines the memory offset of the application code
 */
void flash_exec_cmd(char *str);
#endif // QSPI_FLASH_H__
