/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/USB/Device/USB_MSC_Flash/msc.h
 *  @author     Baikal electronics SDK team
 *  @brief      USB MSC functions header file
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

#ifndef __MSC_H
#define __MSC_H

#include <stdint.h>

void msc_init(void);
uint32_t msc_sync(void);
uint8_t msc_get_modified_flag(void);
void msc_set_write_time(uint64_t write_time);
uint64_t msc_get_write_time(void);

#endif /* __MSC_H */
