/**
 * *****************************************************************************
 *  @file       board.h
 *  @author     Baikal electronics SDK team
 *  @brief      Baikal MCU USB driver
 *  @version    2.3.0
 *  @date       20260703
 * *****************************************************************************
 *  @copyright Copyright (c) 2026 Baikal Electronics JSC
 *
 *  This software is licensed under terms that can be found in the LICENSE file
 *  in the root directory of this software component.
 *  If no LICENSE file comes with this software, it is provided AS-IS.
 * *****************************************************************************
 */

#ifndef __BOARD_H
#define __BOARD_H

#include "bmcu_common.h"
#include "bsp.h"

#ifndef USB_INT_LEVEL
#define USB_INT_LEVEL               (1U)    /*!< USB interrupt level */
#endif /* USB_INT_LEVEL */

#ifndef USB_INT_PRIORITY
#define USB_INT_PRIORITY            (1U)    /*!< USB interrupt priority */
#endif /* USB_INT_PRIORITY */

#ifndef USB_DMA_INT_LEVEL
#define USB_DMA_INT_LEVEL           (1U)    /*!< USB DMA interrupt level */
#endif /* USB_DMA_INT_LEVEL */

#ifndef USB_DMA_INT_PRIORITY
#define USB_DMA_INT_PRIORITY        (1U)    /*!< USB DMA interrupt priority */
#endif /* USB_DMA_INT_PRIORITY */

#endif /* __BOARD_H */
