/**
 * *****************************************************************************
 *  @file       Projects/_template/module2/src/module2_2.c
 *  @author     Baikal electronics SDK team
 *  @brief      Template example source file
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
#include "module2.h"

static volatile int var = 0xA5A5;

int
module2_2_func (void) {
    return (module2_1_func() + var + 3137);
}
