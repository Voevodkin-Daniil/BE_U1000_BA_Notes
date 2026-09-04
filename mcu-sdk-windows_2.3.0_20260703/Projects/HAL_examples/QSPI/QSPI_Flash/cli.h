/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/QSPI/QSPI_Flash/cli.h
 *  @author     Baikal electronics SDK team
 *  @brief      Command line interface header file
 *  @version    2.3.0
 *  @date       20260703
 * 
 *  File contains CLI related functions prototypes
 * *****************************************************************************
 *  @copyright Copyright (c) 2025 Baikal Electronics JSC
 *
 *  This software is licensed under terms that can be found in the LICENSE file
 *  in the root directory of this software component.
 *  If no LICENSE file comes with this software, it is provided AS-IS.
 * *****************************************************************************
 */

#ifndef __CLI_H__
#define __CLI_H__
#include <stdint.h>

/**
 * @brief   CLI initialisation
 *          Prepare CLI functionality and print the CLI header
 *          Must be executed before calling other CLI functions
 * @param   none
 * @retval  none
 */
void cli_init (void);

/**
 * @brief   UART character receiving callback function
 *          Interprete received character and control CLI accordingly
 * @param   rx_char received character value
 * @retval  none
 */
void cli_char_rx_callback(uint8_t rx_char);

/**
 * @brief   Check if the sting is affirmative or not
 * @param   str the pointer to the string being checked
 * @note    case insensitive
 * @retval  1 if the argument string is affirmative,
 *          otherwise 0
 */
int cli_is_param_affirmative(char *str);

/**
 * @brief   Convert HEX-format string to the integer value
 * @param   str the pointer to the string to be converted
 * @note    string can begin by '0x' prefix, which will be skipped
 * @retval  integer value of the converted input HEX-format string
 */
uint32_t cli_param_strtohex(char *str);

/**
 * @brief   Get the pointerto the n-th parameter by its number
 * @param   str pointer to the arguments string
 * @param   n number of the desired parameter starting from 0
 * @retval  pointer to the n-th parameter in the parameters string,
 *          or NULL if such parameter was not found
 */
char *cli_get_param_ptr(char *str, int n);

/**
 * @brief   Print the buffer content in HEX and charaster representation
 *    Addr/offset| 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F  | 0123456789ABCDEF
 *    +----------+-------------------------------------------------+-----------------
 *    startval   | xx xx xx xx xx xx xx xx xx xx xx xx xx xx xx xx | ................
 *    startval+16| xx xx xx xx xx xx xx xx xx xx xx xx xx xx xx xx | ................
 *    etc...
 * @param   buf pointer to the buffer to be printed
 * @param   len length of the data buffer to be printed
 * @param   startval numeric value to be used as the start address of the buffer
 *          (affects the "address" column)
 * @retval  none
 */
void cli_hexdump(uint8_t *buf, uint32_t len, uint32_t startval);

#endif // __CLI_H__
