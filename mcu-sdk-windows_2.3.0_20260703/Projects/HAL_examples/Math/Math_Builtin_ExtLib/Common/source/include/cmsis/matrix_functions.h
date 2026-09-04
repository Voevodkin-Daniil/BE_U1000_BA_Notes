/*****************************************************************************
 * @brief Declaration of all CMSIS-DSP compatible matrix functions
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

#ifndef _MATRIX_FUNCTIONS_H_
#define _MATRIX_FUNCTIONS_H_

#include "bear_dsp_math_types.h"

#ifdef   __cplusplus
extern "C"
{
#endif

/**
 * @brief Instance structure for the floating-point matrix structure.
 */
typedef struct {
    uint16_t num_rows;  /**< number of rows of the matrix.     */
    uint16_t num_clmns; /**< number of columns of the matrix.  */
    float32_t* data;    /**< points to the data of the matrix. */
} bear_matrix_instance_f32;

/**
 * @brief Instance structure for the Q15 matrix structure.
 */
typedef struct {
    uint16_t num_rows;  /**< number of rows of the matrix.     */
    uint16_t num_clmns; /**< number of columns of the matrix.  */
    q15_t* data;        /**< points to the data of the matrix. */
} bear_matrix_instance_q15;

/**
 * @brief  Floating-point matrix initialization.
 * @param[in,out] inst     points to an instance of the floating-point
 *                         matrix structure.
 * @param[in]     rows     number of rows in the matrix.
 * @param[in]     columns  number of columns in the matrix.
 * @param[in]     data     points to the matrix data array.
 */
void bear_mat_init_f32(bear_matrix_instance_f32* inst, uint16_t rows,
                       uint16_t columns, float32_t* data);

/**
 * @brief  Q15 matrix initialization.
 * @param[in,out] inst     points to an instance of the q15 matrix structure.
 * @param[in]     rows     number of rows in the matrix.
 * @param[in]     columns  number of columns in the matrix.
 * @param[in]     data     points to the matrix data array.
 */
void bear_mat_init_q15(bear_matrix_instance_q15* inst, uint16_t rows,
                       uint16_t columns, q15_t* data);

/**
 * @brief Floating-point matrix addition.
 * @param[in]  src_a  points to the first input matrix structure
 * @param[in]  src_b  points to the second input matrix structure
 * @param[out] dst    points to output matrix structure
 * @return     The function returns either
 *             BEAR_MATH_SIZE_MISMATCH or BEAR_MATH_SUCCESS based on
 *             the outcome of size checking.
 */
bear_status_t bear_mat_add_f32(const bear_matrix_instance_f32* src_a,
                               const bear_matrix_instance_f32* src_b,
                               bear_matrix_instance_f32* dst);

/**
 * @brief Floating-point matrix transpose.
 * @param[in]  src  points to the input matrix
 * @param[out] dst  points to the output matrix
 * @return     The function returns either  BEAR_MATH_SIZE_MISMATCH
 *             or BEAR_MATH_SUCCESS based on the outcome of size checking.
 */
bear_status_t bear_mat_trans_f32(const bear_matrix_instance_f32* src,
                                 bear_matrix_instance_f32* dst);

/**
 * @brief Floating-point matrix multiplication
 * @param[in]  src_a  points to the first input matrix structure
 * @param[in]  src_b  points to the second input matrix structure
 * @param[out] dst   points to output matrix structure
 * @return     The function returns either
 *             BEAR_MATH_SIZE_MISMATCH or BEAR_MATH_SUCCESS based on
 *             the outcome of size checking.
 */
bear_status_t bear_mat_mult_f32(const bear_matrix_instance_f32* src_a,
                                const bear_matrix_instance_f32* src_b,
                                bear_matrix_instance_f32* dst);

/**
 * @brief Q15 matrix and vector multiplication
 * @param[in]  src_mat   points to the input matrix structure
 * @param[in]  src_vec   points to vector
 * @param[out] dst_vec   points to output vector
 */
void bear_mat_vec_mult_q15(const bear_matrix_instance_q15* src_mat,
                           const q15_t* src_vec, q15_t* dst_vec);

/**
 * @brief Floating-point matrix subtraction
 * @param[in]  src_a  points to the first input matrix structure
 * @param[in]  src_b  points to the second input matrix structure
 * @param[out] dst   points to output matrix structure
 * @return     The function returns either
 *             BEAR_MATH_SIZE_MISMATCH or BEAR_MATH_SUCCESS based on
 *             the outcome of size checking.
 */
bear_status_t bear_mat_sub_f32(const bear_matrix_instance_f32* src_a,
                               const bear_matrix_instance_f32* src_b,
                               bear_matrix_instance_f32* dst);

/**
 * @brief Floating-point matrix scaling.
 * @param[in]  src   points to the input matrix
 * @param[in]  scale  scale factor
 * @param[out] dst   points to the output matrix
 * @return     The function returns either
 *             BEAR_MATH_SIZE_MISMATCH or BEAR_MATH_SUCCESS based on
 *             the outcome of size checking.
 */
bear_status_t bear_mat_scale_f32(const bear_matrix_instance_f32* src,
                                 float32_t scale,
                                 bear_matrix_instance_f32* dst);

/**
 * @brief Floating-point matrix inverse.
 * @param[in]  src   points to the instance of the input matrix
 * @param[out] dst   points to the instance of the output matrix
 * @return     The function returns BEAR_MATH_SIZE_MISMATCH, if the dimensions
 *             do not match. If the input matrix is singular (does not have an
 *             inverse), then the algorithm terminates and returns error status
 *             BEAR_MATH_SINGULAR.
 */
bear_status_t bear_mat_inverse_f32(const bear_matrix_instance_f32* src,
                                   bear_matrix_instance_f32* dst);

#ifdef   __cplusplus
}
#endif

#endif /* ifndef _MATRIX_FUNCTIONS_H_ */
