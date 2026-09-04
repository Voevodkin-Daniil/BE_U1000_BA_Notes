/**
 * *****************************************************************************
 *  @file       hcd_bmcu.h
 *  @author     Baikal electronics SDK team
 *  @brief      Baikal MCU USB Host driver header file
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

#ifndef __HCD_BMCU_H
#define __HCD_BMCU_H

#if defined (BE_U1000)

#ifndef USBH_USE_DMA
#define USBH_USE_DMA                (1U)    /*!< Use USB DMA */

                                            /* NOTE 1: take in account that the number of DMA channels is limited.
                                                       A DMA channel is assigned for a Rx or Tx endpoint. It should
                                                       suffice for a single device connected, but not for a hub.
                                            */

                                            /* NOTE 2: DMA channels are assigned to endpoints in the order of creation
                                                       up to the maximal number. DMA channel 0 is reserved for the EP0.
                                            */
#endif /* USBH_USE_DMA */

#endif /* BE_U1000 */

#ifndef USBH_EP_MAX
#define USBH_EP_MAX                 (6U)    /*!< The maximum number of endpoints (control and non-control) */
#endif /* USBH_EP_MAX */

#endif /* __HCD_BMCU_H */
