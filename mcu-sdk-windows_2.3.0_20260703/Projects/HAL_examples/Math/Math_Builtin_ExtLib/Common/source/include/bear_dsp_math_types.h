/*****************************************************************************
 * @brief Public header with platform and compiler specific code definitions.
 *
 * @author dmitry.zakharov@cloudbear.ru
 *
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
 *****************************************************************************/

#ifndef _BEAR_DSP_MATH_TYPES_H_
#define _BEAR_DSP_MATH_TYPES_H_

#include <limits.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 8-bit fractional data type in 1.7 format.
 */
typedef int8_t q7_t;

/**
 * @brief 16-bit fractional data type in 1.15 format.
 */
typedef int16_t q15_t;

/**
 * @brief 32-bit fractional data type in 1.31 format.
 */
typedef int32_t q31_t;

/**
 * @brief 64-bit fractional data type in 1.63 format.
 */
typedef int64_t q63_t;

/**
 * @brief 32-bit floating-point type definition.
 */
typedef float float32_t;

/**
 * @brief 64-bit floating-point type definition.
 */
typedef double float64_t;

#define F64_MAX ((float64_t)DBL_MAX)
#define F32_MAX ((float32_t)FLT_MAX)

#define F64_MIN (-DBL_MAX)
#define F32_MIN (-FLT_MAX)

#define F64_ABSMAX ((float64_t)DBL_MAX)
#define F32_ABSMAX ((float32_t)FLT_MAX)

#define F64_ABSMIN ((float64_t)0.0)
#define F32_ABSMIN ((float32_t)0.0)

#define Q31_MAX ((q31_t)(0x7FFFFFFFL))
#define Q15_MAX ((q15_t)(0x7FFF))
#define Q7_MAX ((q7_t)(0x7F))
#define Q31_MIN ((q31_t)(0x80000000L))
#define Q15_MIN ((q15_t)(0x8000))
#define Q7_MIN ((q7_t)(0x80))

#define Q31_ABSMAX ((q31_t)(0x7FFFFFFFL))
#define Q15_ABSMAX ((q15_t)(0x7FFF))
#define Q7_ABSMAX ((q7_t)(0x7F))
#define Q31_ABSMIN ((q31_t)0)
#define Q15_ABSMIN ((q15_t)0)
#define Q7_ABSMIN ((q7_t)0)

/* Dimension C vector space */
#define CMPLX_DIM 2

/**
 * @brief Error status returned by some functions in the library.
 */

typedef enum {
    BEAR_MATH_SUCCESS = 0,         /**< No error */
    BEAR_MATH_ARGUMENT_ERROR = -1, /**< One or more arguments are incorrect */
    BEAR_MATH_LENGTH_ERROR = -2,   /**< Length of data buffer is incorrect */
    BEAR_MATH_SIZE_MISMATCH = -3,  /**< Size of matrices is not compatible with
                                        the operation */
    BEAR_MATH_NANINF = -4,         /**< Not-a-number (NaN) or infinity is
                                        generated */
    BEAR_MATH_SINGULAR = -5,       /**< Input matrix is singular and cannot be
                                        inverted */
    BEAR_MATH_TEST_FAILURE = -6,   /**< Test Failed */
    BEAR_MATH_DECOMPOSITION_FAILURE = -7 /**< Decomposition Failed */
} bear_status_t;

/**
 * @brief Vector type of 8 signed 8-bit values.
 */
typedef int64_t v8int8_t;

/**
 * @brief Vector type of 4 signed 8-bit values.
 */
typedef int32_t v4int8_t;

/**
 * @brief Vector type of 4 signed 16-bit values.
 */
typedef int64_t v4int16_t;

/**
 * @brief Vector type of 2 signed 16-bit values.
 */
typedef int32_t v2int16_t;

/**
 * @brief Vector type of 2 signed 32-bit values.
 */
typedef int64_t v2int32_t;

#ifdef __cplusplus
}
#endif

#endif /*ifndef _BEAR_DSP_MATH_TYPES_H_ */
