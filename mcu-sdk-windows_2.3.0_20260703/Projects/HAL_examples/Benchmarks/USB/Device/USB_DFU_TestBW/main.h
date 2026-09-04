/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/Benchmarks/USB/Device/USB_DFU_TestBW/main.h
 *  @author     Baikal electronics SDK team
 *  @brief      USB DFU class example header file
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

#ifndef __MAIN_H__
#define __MAIN_H__
#include <stdint.h>
#include "dfu.h"

void cmd_usbinit(char *args);
void cmd_usbdeinit(char *args);

#endif // __MAIN_H__
