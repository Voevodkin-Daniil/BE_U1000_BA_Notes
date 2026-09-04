/**
 * *****************************************************************************
 *  @file       Projects/_template/module2/src/module2_1.c
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

int
module2_1_func (void) {
    static int var = 0x5A5A;
    return CONST2 + var + 42;
}
