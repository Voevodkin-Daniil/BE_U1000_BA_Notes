/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/Benchmarks/USB/Device/USB_DFU_TestBW/qspi.h
 *  @author     Baikal electronics SDK team
 *  @brief      MT25QL128 QSPI flash memory access driver header file
 *  @version    2.3.0
 *  @date       20260703
 *
 * *****************************************************************************
 *  @copyright Copyright (c) 2026 Baikal Electronics JSC
 *
 *  This software is licensed under terms that can be found in the LICENSE file
 *  in the root directory of this software component.
 *  If no LICENSE file comes with this software, it is provided AS-IS.
 * *****************************************************************************
 */

#ifndef QSPI_H__
#define QSPI_H__
#include "bmcu_qspi.h"

void qspi_init(void);
void qspi_flash_erase(void);
void qspi_flash_set_quad(FunctionalState state);
void qspi_flash_read(uint32_t addr, uint8_t *data, uint16_t size);
void qspi_flash_write(uint32_t addr, const uint8_t *data, uint16_t size);
#endif // QSPI_H__
