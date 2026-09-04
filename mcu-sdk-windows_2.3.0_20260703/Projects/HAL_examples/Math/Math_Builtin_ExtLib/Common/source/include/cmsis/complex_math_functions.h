/*****************************************************************************
 * @brief Declaration of all CMSIS-DSP complex math functions
 *****************************************************************************
 * Copyright (C) 2010-2021 ARM Limited or its affiliates. All rights reserved.
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

#ifndef _COMPLEX_MATH_FUNCTIONS_H_
#define _COMPLEX_MATH_FUNCTIONS_H_

#include "bear_dsp_math_types.h"

#ifdef   __cplusplus
extern "C"
{
#endif

/**
 * @defgroup groupCmplxMath Complex Math Functions
 * This set of functions operates on complex data vectors.
 * The data in the complex arrays is stored in an interleaved fashion
 * (real, imag, real, imag, ...).
 * In the API functions, the number of samples in a complex array refers
 * to the number of complex values; the array contains twice this number of
 * real values.
 */

/**
 * @brief  Q15 complex magnitude
 * @param[in]  src        points to the complex input vector
 * @param[out] dst        points to the real output vector
 * @param[in]  num_smpls  number of complex samples in the input vector
 */
void bear_cmplx_mag_q15(const q15_t* src, q15_t* dst, uint32_t num_smpls);

#ifdef   __cplusplus
}
#endif

#endif /* ifndef _COMPLEX_MATH_FUNCTIONS_H_ */
