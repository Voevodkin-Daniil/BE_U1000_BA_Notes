/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/USB/Device/USB_DFU/logs.h
 *  @author     Baikal electronics SDK team
 *  @brief      Logging header file
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

#ifndef LOGS_H__
#define LOGS_H__

typedef enum {
    VERBOSITY_NO   = 0,
    VERBOSITY_ERR  = 1,
    VERBOSITY_INFO = 2,
    VERBOSITY_DBG  = 3
} verbosity_e;

void logs(verbosity_e verbosity, const char *format,  ...);

#endif // LOGS_H__
