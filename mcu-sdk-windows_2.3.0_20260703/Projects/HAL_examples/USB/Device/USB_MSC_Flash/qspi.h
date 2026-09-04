/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/USB/Device/USB_MSC_Flash/qspi.h
 *  @author     Baikal electronics SDK team
 *  @brief      QSPI driver header file
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

#ifndef __QSPI_H
#define __QSPI_H

#include <stdint.h>
#include "bmcu_common.h"
#include "bmcu_qspi.h"

void QSPI1_Init(void);
void QSPI1_StdToEnh(void);
void QSPI1_Std_Xfer(uint8_t* DataBuf, uint32_t DataLen);
void QSPI1_Enh_Read(uint8_t Inst,
                    uint32_t Addr,
                    QSPI_AddrLen_TypeDef AddrLen,
                    QSPI_WaitCycles_TypeDef DummyCycles,
                    uint8_t* DataBuf,
                    uint32_t DataLen);
void QSPI1_Enh_Write(uint8_t Inst,
                     uint32_t Addr,
                     QSPI_AddrLen_TypeDef AddrLen,
                     uint8_t* DataBuf,
                     uint32_t DataLen);

#endif /* __QSPI_H */
