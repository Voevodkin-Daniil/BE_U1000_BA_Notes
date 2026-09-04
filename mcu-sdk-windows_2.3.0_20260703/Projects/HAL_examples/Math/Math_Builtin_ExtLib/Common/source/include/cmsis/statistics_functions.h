/*****************************************************************************
 * @brief Declaration of all CMSIS-DSP statistics matrix functions
 *****************************************************************************
 * Copyright (C) 2010-2020 ARM Limited or its affiliates. All rights reserved.
 * Copyright (c) 2023 CloudBEAR LLC, all rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef _STATISTICS_FUNCTIONS_H_
#define _STATISTICS_FUNCTIONS_H_

#include "bear_dsp_math_types.h"

#ifdef   __cplusplus
extern "C"
{
#endif


/**
 * @defgroup groupStats Statistics Functions
 */

/**
 * @brief Maximum value of absolute values of a Q15 vector.
 * @param[in]  src         points to the input buffer
 * @param[in]  block_size  length of the input vector
 * @param[out] result      maximum value returned here
 * @param[out] index       index of maximum value returned here
 */
void bear_absmax_q15(const q15_t* src, uint32_t block_size, q15_t* result,
                     uint32_t* index);

#ifdef   __cplusplus
}
#endif

#endif /* ifndef _STATISTICS_FUNCTIONS_H_ */
