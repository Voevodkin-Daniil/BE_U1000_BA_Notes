/*****************************************************************************
 * @brief Declaration of all CMSIS-DSP compatible basic math functions
 *
 *****************************************************************************
 * Copyright (c) 2010-2020 Arm Limited or its affiliates. All rights reserved.
 * Copyright (c) 2022-2023 CloudBEAR LLC, all rights reserved.
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

#ifndef _BASIC_MATH_FUNCTIONS_H_
#define _BASIC_MATH_FUNCTIONS_H_

#include "bear_dsp_math_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Q7 vector multiplication.
 * @param[in]  src_a       points to the first input vector
 * @param[in]  src_b       points to the second input vector
 * @param[out] dst         points to the output vector
 * @param[in]  block_size  number of samples in each vector
 */
void bear_mult_q7(const q7_t* src_a, const q7_t* src_b, q7_t* dst,
                  uint32_t block_size);

/**
 * @brief Q15 vector multiplication.
 * @param[in]  src_a       points to the first input vector
 * @param[in]  src_b       points to the second input vector
 * @param[out] dst         points to the output vector
 * @param[in]  block_size  number of samples in each vector
 */
void bear_mult_q15(const q15_t* src_a, const q15_t* src_b, q15_t* dst,
                   uint32_t block_size);

/**
 * @brief Q31 vector multiplication.
 * @param[in]  src_a       points to the first input vector
 * @param[in]  src_b       points to the second input vector
 * @param[out] dst         points to the output vector
 * @param[in]  block_size  number of samples in each vector
 */
void bear_mult_q31(const q31_t* src_a, const q31_t* src_b, q31_t* dst,
                   uint32_t block_size);

/**
 * @brief Q7 vector addition.
 * @param[in]  src_a       points to the first input vector
 * @param[in]  src_b       points to the second input vector
 * @param[out] dst         points to the output vector
 * @param[in]  block_size  number of samples in each vector
 */
void bear_add_q7(const q7_t* src_a, const q7_t* src_b, q7_t* dst,
                 uint32_t block_size);

/**
 * @brief Q15 vector addition.
 * @param[in]  src_a       points to the first input vector
 * @param[in]  src_b       points to the second input vector
 * @param[out] dst         points to the output vector
 * @param[in]  block_size  number of samples in each vector
 */
void bear_add_q15(const q15_t* src_a, const q15_t* src_b, q15_t* dst,
                  uint32_t block_size);

/**
 * @brief Q31 vector addition.
 * @param[in]  src_a       points to the first input vector
 * @param[in]  src_b       points to the second input vector
 * @param[out] dst         points to the output vector
 * @param[in]  block_size  number of samples in each vector
 */
void bear_add_q31(const q31_t* src_a, const q31_t* src_b, q31_t* dst,
                  uint32_t block_size);

/**
 * @brief Q7 vector subtraction.
 * @param[in]  src_a       points to the first input vector
 * @param[in]  src_b       points to the second input vector
 * @param[out] dst         points to the output vector
 * @param[in]  block_size  number of samples in each vector
 */
void bear_sub_q7(const q7_t* src_a, const q7_t* src_b, q7_t* dst,
                 uint32_t block_size);

/**
 * @brief Q15 vector subtraction.
 * @param[in]  src_a       points to the first input vector
 * @param[in]  src_b       points to the second input vector
 * @param[out] dst         points to the output vector
 * @param[in]  block_size  number of samples in each vector
 */
void bear_sub_q15(const q15_t* src_a, const q15_t* src_b, q15_t* dst,
                  uint32_t block_size);

/**
 * @brief Q31 vector subtraction.
 * @param[in]  src_a       points to the first input vector
 * @param[in]  src_b       points to the second input vector
 * @param[out] dst         points to the output vector
 * @param[in]  block_size  number of samples in each vector
 */
void bear_sub_q31(const q31_t* src_a, const q31_t* src_b, q31_t* dst,
                  uint32_t block_size);

/**
 * @brief Multiplies a Q7 vector by a scalar.
 * @param[in]  src          points to the input vector
 * @param[in]  scale_fract  fractional portion of the scale value
 * @param[in]  shift        number of bits to shift the result by
 * @param[out] dst          points to the output vector
 * @param[in]  block_size   number of samples in the vector
 */
void bear_scale_q7(const q7_t* src, q7_t scale_fract, int8_t shift, q7_t* dst,
                   uint32_t block_size);

/**
 * @brief Multiplies a Q15 vector by a scalar.
 * @param[in]  src          points to the input vector
 * @param[in]  scale_fract  fractional portion of the scale value
 * @param[in]  shift        number of bits to shift the result by
 * @param[out] dst          points to the output vector
 * @param[in]  block_size   number of samples in the vector
 */
void bear_scale_q15(const q15_t* src, q15_t scale_fract, int8_t shift,
                    q15_t* dst, uint32_t block_size);

/**
 * @brief Multiplies a Q31 vector by a scalar.
 * @param[in]  src          points to the input vector
 * @param[in]  scale_fract  fractional portion of the scale value
 * @param[in]  shift        number of bits to shift the result by
 * @param[out] dst          points to the output vector
 * @param[in]  block_size   number of samples in the vector
 */
void bear_scale_q31(const q31_t* src, q31_t scale_fract, int8_t shift,
                    q31_t* dst, uint32_t block_size);

/**
 * @brief Q7 vector absolute value.
 * @param[in]  src         points to the input buffer
 * @param[out] dst         points to the output buffer
 * @param[in]  block_size  number of samples in each vector
 */
void bear_abs_q7(const q7_t* src, q7_t* dst, uint32_t block_size);

/**
 * @brief Q15 vector absolute value.
 * @param[in]  src         points to the input buffer
 * @param[out] dst         points to the output buffer
 * @param[in]  block_size  number of samples in each vector
 */
void bear_abs_q15(const q15_t* src, q15_t* dst, uint32_t block_size);

/**
 * @brief Q31 vector absolute value.
 * @param[in]  src         points to the input buffer
 * @param[out] dst         points to the output buffer
 * @param[in]  block_size  number of samples in each vector
 */
void bear_abs_q31(const q31_t* src, q31_t* dst, uint32_t block_size);

/**
 * @brief Dot product of Q7 vectors.
 * @param[in]  src_a       points to the first input vector
 * @param[in]  src_b       points to the second input vector
 * @param[in]  block_size  number of samples in each vector
 * @param[out] result      output result returned here
 */
void bear_dot_prod_q7(const q7_t* src_a, const q7_t* src_b, uint32_t block_size,
                      q31_t* result);

/**
 * @brief Dot product of Q15 vectors.
 * @param[in]  src_a       points to the first input vector
 * @param[in]  src_b       points to the second input vector
 * @param[in]  block_size  number of samples in each vector
 * @param[out] result      output result returned here
 */
void bear_dot_prod_q15(const q15_t* src_a, const q15_t* src_b,
                       uint32_t block_size, q63_t* result);

/**
 * @brief Dot product of Q31 vectors.
 * @param[in]  src_a       points to the first input vector
 * @param[in]  src_b       points to the second input vector
 * @param[in]  block_size  number of samples in each vector
 * @param[out] result      output result returned here
 */
void bear_dot_prod_q31(const q31_t* src_a, const q31_t* src_b,
                       uint32_t block_size, q63_t* result);

/**
 * @brief Dot product of F32 vectors.
 * @param[in]  src_a       points to the first input vector
 * @param[in]  src_b       points to the second input vector
 * @param[in]  block_size  number of samples in each vector
 * @param[out] result      output result returned here
 */
void bear_dot_prod_f32(const float32_t* src_a, const float32_t* src_b,
                       uint32_t block_size, float32_t* result);

/**
 * @brief  Shifts the elements of a Q7 vector a specified number of bits.
 * @param[in]  src         points to the input vector
 * @param[in]  shift_bits  number of bits to shift. A positive value shifts
 *                         left; a negative value shifts right.
 * @param[out] dst         points to the output vector
 * @param[in]  block_size  number of samples in the vector
 */
void bear_shift_q7(const q7_t* src, int8_t shift_bits, q7_t* dst,
                   uint32_t block_size);

/**
 * @brief  Shifts the elements of a Q15 vector a specified number of bits.
 * @param[in]  src         points to the input vector
 * @param[in]  shift_bits  number of bits to shift. A positive value shifts
 *                         left; a negative value shifts right.
 * @param[out] dst         points to the output vector
 * @param[in]  block_size  number of samples in the vector
 */
void bear_shift_q15(const q15_t* src, int8_t shift_bits, q15_t* dst,
                    uint32_t block_size);

/**
 * @brief  Shifts the elements of a Q31 vector a specified number of bits.
 * @param[in]  src         points to the input vector
 * @param[in]  shift_bits  number of bits to shift. A positive value shifts
 *                         left; a negative value shifts right.
 * @param[out] dst         points to the output vector
 * @param[in]  block_size  number of samples in the vector
 */
void bear_shift_q31(const q31_t* src, int8_t shift_bits, q31_t* dst,
                    uint32_t block_size);

/**
 * @brief  Adds a constant offset to a Q7 vector.
 * @param[in]  src         points to the input vector
 * @param[in]  offset      is the offset to be added
 * @param[out] dst         points to the output vector
 * @param[in]  block_size  number of samples in the vector
 */
void bear_offset_q7(const q7_t* src, q7_t offset, q7_t* dst,
                    uint32_t block_size);

/**
 * @brief  Adds a constant offset to a Q15 vector.
 * @param[in]  src         points to the input vector
 * @param[in]  offset      is the offset to be added
 * @param[out] dst         points to the output vector
 * @param[in]  block_size  number of samples in the vector
 */
void bear_offset_q15(const q15_t* src, q15_t offset, q15_t* dst,
                     uint32_t block_size);

/**
 * @brief  Adds a constant offset to a Q31 vector.
 * @param[in]  src         points to the input vector
 * @param[in]  offset      is the offset to be added
 * @param[out] dst         points to the output vector
 * @param[in]  block_size  number of samples in the vector
 */
void bear_offset_q31(const q31_t* src, q31_t offset, q31_t* dst,
                     uint32_t block_size);

/**
 * @brief  Adds a constant offset to a F32 vector.
 * @param[in]  src         points to the input vector
 * @param[in]  offset      is the offset to be added
 * @param[out] dst         points to the output vector
 * @param[in]  block_size  number of samples in the vector
 */
void bear_offset_f32(const float32_t* src, float32_t offset, float32_t* dst,
                     uint32_t block_size);

/**
 * @brief  Negates the elements of a Q7 vector.
 * @param[in]  src         points to the input vector
 * @param[out] dst         points to the output vector
 * @param[in]  block_size  number of samples in the vector
 */
void bear_negate_q7(const q7_t* src, q7_t* dst, uint32_t block_size);

/**
 * @brief  Negates the elements of a Q15 vector.
 * @param[in]  src         points to the input vector
 * @param[out] dst         points to the output vector
 * @param[in]  block_size  number of samples in the vector
 */
void bear_negate_q15(const q15_t* src, q15_t* dst, uint32_t block_size);

/**
 * @brief  Negates the elements of a Q31 vector.
 * @param[in]  src         points to the input vector
 * @param[out] dst         points to the output vector
 * @param[in]  block_size  number of samples in the vector
 */
void bear_negate_q31(const q31_t* src, q31_t* dst, uint32_t block_size);

/**
 * @brief         Elementwise fixed-point clipping of a Q31 vector
 * @param[in]     src           points to input values
 * @param[out]    dst           points to output clipped values
 * @param[in]     low           lower bound
 * @param[in]     high          higher bound
 * @param[in]     num_samples   number of samples to clip
 * @return        none
 */
void bear_clip_q31(const q31_t* src, q31_t* dst, q31_t low, q31_t high,
                   uint32_t num_samples);

/**
 * @brief         Elementwise fixed-point clipping of a Q15 vector
 * @param[in]     src          points to input values
 * @param[out]    dst          points to output clipped values
 * @param[in]     low          lower bound
 * @param[in]     high         higher bound
 * @param[in]     num_samples  number of samples to clip
 * @return        none
 */
void bear_clip_q15(const q15_t* src, q15_t* dst, q15_t low, q15_t high,
                   uint32_t num_samples);

/**
 * @brief         Elementwise fixed-point clipping of a Q7 vector
 * @param[in]     src          points to input values
 * @param[out]    dst          points to output clipped values
 * @param[in]     low          lower bound
 * @param[in]     high         higher bound
 * @param[in]     num_samples  number of samples to clip
 * @return        none
 */
void bear_clip_q7(const q7_t* src, q7_t* dst, q7_t low, q7_t high,
                  uint32_t num_samples);

#ifdef __cplusplus
}
#endif

#endif /* ifndef _BASIC_MATH_FUNCTIONS_H_ */
