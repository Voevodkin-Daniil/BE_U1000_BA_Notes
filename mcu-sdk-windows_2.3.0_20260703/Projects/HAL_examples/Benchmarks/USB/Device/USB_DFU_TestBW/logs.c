/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/Benchmarks/USB/Device/USB_DFU_TestBW/logs.c
 *  @author     Baikal electronics SDK team
 *  @brief      Logging
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

#include <stdio.h>
#include <stdarg.h>
#include "logs.h"

#ifdef VERBOSITY
static verbosity_e cur_verbosity_level = VERBOSITY;
#else
static verbosity_e cur_verbosity_level = VERBOSITY_INFO;
#endif

void
logs (verbosity_e verbosity, const char *format,  ...) {
    va_list argptr;

    if (verbosity > cur_verbosity_level) {
        return;
    }

    va_start(argptr, format);
    vprintf(format, argptr);
    va_end(argptr);
}
