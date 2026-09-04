/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/Benchmarks/whetstone/source/wrap_printf.h
 *  @author     Baikal electronics SDK team
 *  @brief      HAL example source file
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
#ifndef __WRAP_PRINTF_H
#define __WRAP_PRINTF_H

#include "mutex.h"

extern mutex_t* uart_mutex;

#define PRINTF(...) do { \
    mutex_lock(uart_mutex); \
    printf("Core%lu: ", __get_hart_id()); printf(__VA_ARGS__); \
    mutex_unlock(uart_mutex); \
} while (0)

#endif
