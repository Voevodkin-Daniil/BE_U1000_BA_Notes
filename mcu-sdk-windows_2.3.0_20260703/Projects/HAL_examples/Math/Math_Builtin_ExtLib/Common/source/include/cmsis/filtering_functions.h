/*****************************************************************************
 * @brief Declaration of all CMSIS-DSP compatible filtering functions
 *
 *****************************************************************************
 * Copyright (c) 2010-2020 Arm Limited or its affiliates. All rights reserved.
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
 *****************************************************************************/

#ifndef _FILTERING_FUNCTIONS_H_
#define _FILTERING_FUNCTIONS_H_

#include "bear_dsp_config.h"
#include "bear_dsp_math_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup groupFilters Filtering Functions
 */

/**
 * @brief Instance structure for the Q15 FIR filter.
 */
typedef struct {
    uint16_t num_taps;   /**< number of filter coefficients in the filter. */
    q15_t* state;        /**< points to the state variable array.
                              The array is of length num_taps+block_size-1. */
    const q15_t* coeffs; /**< points to the coefficient array. The array is of
                              length num_taps.*/
} bear_fir_instance_q15;

/**
 * @brief Instance structure for the Q31 FIR filter.
 */
typedef struct {
    uint16_t num_taps;   /**< number of filter coefficients in the filter. */
    q31_t* state;        /**< points to the state variable array.
                              The array is of length num_taps+block_size-1. */
    const q31_t* coeffs; /**< points to the coefficient array. The array is of
                              length num_taps. */
} bear_fir_instance_q31;

/**
 * @brief Processing function for the Q15 FIR filter.
 * @param[in]  inst        points to an instance of the Q15 FIR structure.
 * @param[in]  src         points to the block of input data.
 * @param[out] dst         points to the block of output data.
 * @param[in]  block_size  number of samples to process.
 */
void bear_fir_q15(const bear_fir_instance_q15* inst, const q15_t* src,
                  q15_t* dst, uint32_t block_size);

/**
 * @brief  Initialization function for the Q15 FIR filter.
 * @param[in,out] inst        points to an instance of the Q15 FIR filter
 *                            structure.
 * @param[in]     num_taps    Number of filter coefficients in the filter. Must
 *                            be a multiple of 4. You can pad with zeros if you
 *                            have less coefficients.
 * @param[in]     coeffs      points to the filter coefficients.
 * @param[in]     state       points to the state buffer.
 * @param[in]     block_size  number of samples that are processed at a time.
 * @return        The function returns either
 *                  BEAR_MATH_SUCCESS:        if initialization was successful
 *                  BEAR_MATH_ARGUMENT_ERROR: if <code>num_taps</code> is not a
 *                                            supported value.
 */
bear_status_t bear_fir_init_q15(bear_fir_instance_q15* inst, uint16_t num_taps,
                                const q15_t* coeffs, q15_t* state,
                                uint32_t block_size);

/**
 * @brief Processing function for the Q31 FIR filter.
 * @param[in]  inst        points to an instance of the Q31 FIR filter
 *                         structure.
 * @param[in]  src         points to the block of input data.
 * @param[out] dst         points to the block of output data.
 * @param[in]  block_size  number of samples to process.
 */
void bear_fir_q31(const bear_fir_instance_q31* inst, const q31_t* src,
                  q31_t* dst, uint32_t block_size);

/**
 * @brief  Initialization function for the Q31 FIR filter.
 * @param[in,out] inst        points to an instance of the Q31 FIR structure.
 * @param[in]     num_taps    Number of filter coefficients in the filter. Must
 *                            be a multiple of 2. You can pad with zeros if you
 *                            have less coefficients.
 * @param[in]     coeffs      points to the filter coefficients.
 * @param[in]     state       points to the state buffer.
 * @param[in]     block_size  number of samples that are processed at a time.
 */
void bear_fir_init_q31(bear_fir_instance_q31* inst, uint16_t num_taps,
                       const q31_t* coeffs, q31_t* state, uint32_t block_size);

/**
 * @brief Instance structure for the Q15 Biquad cascade filter.
 */
typedef struct {
    int8_t num_stages;   /**< Number of 2nd order stages in the filter.
                              Overall order is 2*num_stages. */
    q15_t* state;        /**< Points to the array of state coefficients.
                              The array is of length 4*num_stages. */
    const q15_t* coeffs; /**< Points to the array of coefficients.
                              The array is of length 5*num_stages. */
    int8_t post_shift;   /**< Additional shift, in bits, applied to each
                              output sample. */
} bear_biquad_casd_df1_inst_q15;

/**
 * @brief Instance structure for the Q31 Biquad cascade filter.
 */
typedef struct {
    uint32_t num_stages; /**< number of 2nd order stages in the filter.
                              Overall order is 2*num_stages. */
    q31_t* state;        /**< Points to the array of state coefficients.
                              The array is of length 4*num_stages. */
    const q31_t* coeffs; /**< Points to the array of coefficients.
                              The array is of length 5*num_stages. */
    uint8_t post_shift;  /**< Additional shift, in bits, applied to each
                              output sample. */
} bear_biquad_casd_df1_inst_q31;

/**
 * @brief Instance structure for the floating-point Biquad cascade filter.
 */
typedef struct {
    uint32_t num_stages;     /**< number of 2nd order stages in the filter.
                                  Overall order is 2*num_stages. */
    float32_t* state;        /**< Points to the array of state coefficients.
                                  The array is of length 4*num_stages. */
    const float32_t* coeffs; /**< Points to the array of coefficients.
                                  Thearray is of length 5*num_stages. */
} bear_biquad_casd_df1_inst_f32;

/**
 * @brief Processing function for the Q15 Biquad cascade filter.
 * @param[in]  inst        points to an instance of the Q15 Biquad cascade
 *                         structure.
 * @param[in]  src         points to the block of input data.
 * @param[out] dst         points to the block of output data.
 * @param[in]  block_size  number of samples to process.
 */
void bear_biquad_cascade_df1_q15(const bear_biquad_casd_df1_inst_q15* inst,
                                 const q15_t* src, q15_t* dst,
                                 uint32_t block_size);

/**
 * @brief  Initialization function for the Q15 Biquad cascade filter.
 * @param[in,out] inst        points to an instance of the Q15 Biquad cascade
 *                            structure.
 * @param[in]     num_stages  number of 2nd order stages in the filter.
 * @param[in]     coeffs      points to the filter coefficients.
 * @param[in]     state       points to the state buffer.
 * @param[in]     post_shift  Shift to be applied to the output. Varies
 * according to the coefficients format
 */
void bear_biquad_cascade_df1_init_q15(bear_biquad_casd_df1_inst_q15* inst,
                                      uint8_t num_stages, const q15_t* coeffs,
                                      q15_t* state, int8_t post_shift);

/**
 * @brief Processing function for the Q31 Biquad cascade filter
 * @param[in]  inst        points to an instance of the Q31 Biquad cascade
 *                         structure.
 * @param[in]  src         points to the block of input data.
 * @param[out] dst         points to the block of output data.
 * @param[in]  block_size  number of samples to process.
 */
void bear_biquad_cascade_df1_q31(const bear_biquad_casd_df1_inst_q31* inst,
                                 const q31_t* src, q31_t* dst,
                                 uint32_t block_size);

/**
 * @brief  Initialization function for the Q31 Biquad cascade filter.
 * @param[in,out] inst        points to an instance of the Q31 Biquad cascade
 *                            structure.
 * @param[in]     num_stages  number of 2nd order stages in the filter.
 * @param[in]     coeffs      points to the filter coefficients.
 * @param[in]     state       points to the state buffer.
 * @param[in]     post_shift  Shift to be applied to the output. Varies
 * according to the coefficients format
 */
void bear_biquad_cascade_df1_init_q31(bear_biquad_casd_df1_inst_q31* inst,
                                      uint8_t num_stages, const q31_t* coeffs,
                                      q31_t* state, int8_t post_shift);

/**
 * @brief Processing function for the floating-point Biquad cascade filter.
 * @param[in]  inst        points to an instance of the floating-point Biquad
 *                         cascade structure.
 * @param[in]  src         points to the block of input data.
 * @param[out] dst         points to the block of output data.
 * @param[in]  block_size  number of samples to process.
 */
void bear_biquad_cascade_df1_f32(const bear_biquad_casd_df1_inst_f32* inst,
                                 const float32_t* src, float32_t* dst,
                                 uint32_t block_size);

/**
 * @brief  Initialization function for the floating-point Biquad cascade filter.
 * @param[in,out] inst        points to an instance of the floating-point
 *                            Biquad cascade structure.
 * @param[in]     num_stages  number of 2nd order stages in the filter.
 * @param[in]     coeffs      points to the filter coefficients.
 * @param[in]     state       points to the state buffer.
 */
void bear_biquad_cascade_df1_init_f32(bear_biquad_casd_df1_inst_f32* inst,
                                      uint8_t num_stages,
                                      const float32_t* coeffs,
                                      float32_t* state);

/**
 * @brief Instance structure for the high precision Q31 Biquad cascade filter.
 */
typedef struct {
    uint8_t num_stages;  /**< number of 2nd order stages in the filter.
                                Overall order is 2*num_stages. */
    q63_t* state;        /**< points to the array of state coefficients.
                                  The array is of length 4*num_stages. */
    const q31_t* coeffs; /**< points to the array of coefficients.
                                  The array is of length 5*num_stages. */
    uint8_t post_shift;  /**< additional shift, in bits, applied to each
                                output sample. */
} bear_biquad_cas_df1_32x64_ins_q31;

/**
 * @param[in]  inst        points to an instance of the high precision Q31
 *                         Biquad cascade filter structure.
 * @param[in]  src         points to the block of input data.
 * @param[out] dst         points to the block of output data
 * @param[in]  block_size  number of samples to process.
 */
void bear_biquad_cas_df1_32x64_q31(
    const bear_biquad_cas_df1_32x64_ins_q31* inst, const q31_t* src, q31_t* dst,
    uint32_t block_size);

/**
 * @param[in,out] inst        points to an instance of the high precision Q31
 *                            Biquad cascade filter structure.
 * @param[in]     num_stages  number of 2nd order stages in the filter.
 * @param[in]     coeffs      points to the filter coefficients.
 * @param[in]     state       points to the state buffer.
 * @param[in]     post_shift  shift to be applied to the output. Varies
 * according to the coefficients format
 */
void bear_biquad_cas_df1_32x64_init_q31(bear_biquad_cas_df1_32x64_ins_q31* inst,
                                        uint8_t num_stages, const q31_t* coeffs,
                                        q63_t* state, uint8_t post_shift);

/**
 * @brief Instance structure for the floating-point transposed direct form II
 * Biquad cascade filter.
 */
typedef struct {
    uint8_t num_stages;      /**< number of 2nd order stages in the filter.
                                  Overall order is 2*num_stages. */
    float32_t* state;        /**< points to the array of state coefficients.
                                  The array is of length 2*num_stages. */
    const float32_t* coeffs; /**< points to the array of coefficients.  The
                                  array is of length 5*num_stages. */
} bear_biquad_cascade_df2T_instance_f32;

/**
 * @brief Instance structure for the floating-point transposed direct form II
 * Biquad cascade filter.
 */
typedef struct {
    uint8_t num_stages;      /**< number of 2nd order stages in the filter.
                                  Overall order is 2*num_stages. */
    float32_t* state;        /**< points to the array of state coefficients.
                                  The array is of length 4*num_stages. */
    const float32_t* coeffs; /**< points to the array of coefficients.
                                  The array is of length 5*num_stages. */
} bear_biquad_cascade_stereo_df2T_instance_f32;

/**
 * @brief  Initialization function for the floating-point transposed
 *         direct form II Biquad cascade filter.
 * @param[in,out] inst          points to an instance of the filter data
 *                              structure.
 * @param[in]     num_stages    number of 2nd order stages in the filter.
 * @param[in]     coeffs        points to the filter coefficients.
 * @param[in]     state         points to the state buffer.
 */
void bear_biquad_cascade_df2T_init_f32(
    bear_biquad_cascade_df2T_instance_f32* inst, uint8_t num_stages,
    const float32_t* coeffs, float32_t* state);

/**
 * @brief  Initialization function for the floating-point transposed
 *         direct form II Biquad cascade filter.
 * @param[in,out] inst          points to an instance of the filter data
 *                              structure.
 * @param[in]     num_stages    number of 2nd order stages in the filter.
 * @param[in]     coeffs        points to the filter coefficients.
 * @param[in]     state         points to the state buffer.
 */
void bear_biquad_cascade_stereo_df2T_init_f32(
    bear_biquad_cascade_stereo_df2T_instance_f32* inst, uint8_t num_stages,
    const float32_t* coeffs, float32_t* state);

/**
 * @brief Processing function for the floating-point
 *        transposed direct form II biquad cascade filter.
 * @param[in]  inst        points to an instance of the filter data structure.
 * @param[in]  src         points to the block of input data.
 * @param[out] dst         points to the block of output data
 * @param[in]  block_size  number of samples to process.
 */
void bear_biquad_cascade_df2T_f32(
    const bear_biquad_cascade_df2T_instance_f32* inst, const float32_t* src,
    float32_t* dst, uint32_t block_size);

/**
 * @brief Processing function for the floating-point transposed
 *        direct form II Biquad cascade filter. 2 channels
 * @param[in]  inst         points to an instance of the filter data structure.
 * @param[in]  src          points to the block of input data.
 * @param[out] dst          points to the block of output data
 * @param[in]  block_size   number of samples to process.
 */
void bear_biquad_cascade_stereo_df2T_f32(
    const bear_biquad_cascade_stereo_df2T_instance_f32* inst,
    const float32_t* src, float32_t* dst, uint32_t block_size);

#ifdef __cplusplus
}
#endif

#endif /* ifndef _FILTERING_FUNCTIONS_H_ */
