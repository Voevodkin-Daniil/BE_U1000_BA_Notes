/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/USB/Device/USB_DFU/dfu.h
 *  @author     Baikal electronics SDK team
 *  @brief      USB DFU class FSM header file
 *  @version    2.3.0
 *  @date       20260703
 *
 * *****************************************************************************
 *  @copyright Copyright (c) 2025 Baikal Electronics JSC
 *
 *  This software is licensed under terms that can be found in the LICENSE file
 *  in the root directory of this software component.
 *  If no LICENSE file comes with this software, it is provided AS-IS.
 * *****************************************************************************
 */

#ifndef __DFU_H_
#define __DFU_H_
#include "bmcu_common.h"
#include "logs.h"

int dfu_init(void);
int dfu_deinit(void);
int dfu_exec(void);

#endif //__DFU_H_
