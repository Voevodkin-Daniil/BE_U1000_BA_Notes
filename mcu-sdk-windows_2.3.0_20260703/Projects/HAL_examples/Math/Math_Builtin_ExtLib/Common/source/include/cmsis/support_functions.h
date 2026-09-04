/*****************************************************************************
 * @brief Declaration of all CMSIS-DSP compatible support functions
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

#ifndef _SUPPORT_FUNCTIONS_H_
#define _SUPPORT_FUNCTIONS_H_

#include "bear_dsp_math_types.h"

#ifdef   __cplusplus
extern "C"
{
#endif

/**
 * @defgroup groupSupport Support Functions
 */

/**
 * @brief Converts the elements of the floating-point vector to Q31 vector.
 * @param[in]  src       points to the floating-point input vector
 * @param[out] dst       points to the Q31 output vector
 * @param[in]  block_size  length of the input vector
 */
void bear_float_to_q31(const float32_t* src, q31_t* dst, uint32_t block_size);

/**
 * @brief Converts the elements of the floating-point vector to Q15 vector.
 * @param[in]  src       points to the floating-point input vector
 * @param[out] dst       points to the Q15 output vector
 * @param[in]  block_size  length of the input vector
 */
void bear_float_to_q15(const float32_t* src, q15_t* dst, uint32_t block_size);

/**
 * @brief Converts the elements of the floating-point vector to Q7 vector.
 * @param[in]  src       points to the floating-point input vector
 * @param[out] dst       points to the Q7 output vector
 * @param[in]  block_size  length of the input vector
 */
void bear_float_to_q7(const float32_t* src, q7_t* dst, uint32_t block_size);

/**
 * @brief  Converts the elements of the Q31 vector to floating-point vector.
 * @param[in]  src       is input pointer
 * @param[out] dst       is output pointer
 * @param[in]  block_size  is the number of samples to process
 */
void bear_q31_to_float(const q31_t* src, float32_t* dst, uint32_t block_size);

/**
 * @brief  Converts the elements of the Q31 vector to Q15 vector.
 * @param[in]  src       is input pointer
 * @param[out] dst       is output pointer
 * @param[in]  block_size  is the number of samples to process
 */
void bear_q31_to_q15(const q31_t* src, q15_t* dst, uint32_t block_size);

/**
 * @brief  Converts the elements of the Q31 vector to Q7 vector.
 * @param[in]  src       is input pointer
 * @param[out] dst       is output pointer
 * @param[in]  block_size  is the number of samples to process
 */
void bear_q31_to_q7(const q31_t* src, q7_t* dst, uint32_t block_size);

/**
 * @brief  Converts the elements of the Q15 vector to floating-point vector.
 * @param[in]  src       is input pointer
 * @param[out] dst       is output pointer
 * @param[in]  block_size  is the number of samples to process
 */
void bear_q15_to_float(const q15_t* src, float32_t* dst, uint32_t block_size);

/**
 * @brief  Converts the elements of the Q15 vector to Q31 vector.
 * @param[in]  src       is input pointer
 * @param[out] dst       is output pointer
 * @param[in]  block_size  is the number of samples to process
 */
void bear_q15_to_q31(const q15_t* src, q31_t* dst, uint32_t block_size);

/**
 * @brief  Converts the elements of the Q15 vector to Q7 vector.
 * @param[in]  src       is input pointer
 * @param[out] dst       is output pointer
 * @param[in]  block_size  is the number of samples to process
 */
void bear_q15_to_q7(const q15_t* src, q7_t* dst, uint32_t block_size);

/**
 * @brief  Converts the elements of the Q7 vector to floating-point vector.
 * @param[in]  src       is input pointer
 * @param[out] dst       is output pointer
 * @param[in]  block_size  is the number of samples to process
 */
void bear_q7_to_float(const q7_t* src, float32_t* dst, uint32_t block_size);

/**
 * @brief  Converts the elements of the Q7 vector to Q31 vector.
 * @param[in]  src       input pointer
 * @param[out] dst       output pointer
 * @param[in]  block_size  number of samples to process
 */
void bear_q7_to_q31(const q7_t* src, q31_t* dst, uint32_t block_size);

/**
 * @brief  Converts the elements of the Q7 vector to Q15 vector.
 * @param[in]  src       input pointer
 * @param[out] dst       output pointer
 * @param[in]  block_size  number of samples to process
 */
void bear_q7_to_q15(const q7_t* src, q15_t* dst, uint32_t block_size);

#ifdef   __cplusplus
}
#endif

#endif /* ifndef _SUPPORT_FUNCTIONS_H_ */
