/*****************************************************************************
 * @brief Declaration of all CMSIS-DSP compatible transform functions
 *
 *****************************************************************************
 * Copyright (c) 2010-2023 Arm Limited or its affiliates. All rights reserved.
 * Copyright (c) 2023-2024 CloudBEAR LLC, all rights reserved.
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

#ifndef _TRANSFORM_FUNCTIONS_H_
#define _TRANSFORM_FUNCTIONS_H_

#include "bear_dsp_config.h"
#include "bear_dsp_math_types.h"

#ifdef   __cplusplus
extern "C"
{
#endif

/**
 * @brief Instance structure for the Q15 CFFT/CIFFT function.
 */
typedef struct {
    const q15_t* twiddle_table;   /**< A pointer to the twiddle factor table */
    const uint16_t* bitrev_table; /**< A pointer to the bit reversal table */
    uint16_t fft_len;             /**< FFT length */
    uint16_t bitrev_len;          /**< Bit reversal table length */
} bear_cfft_instance_q15;

/**
 * @brief  Pre-initialized constant instance for the 16 pt Q15 CFFT/CIFFT
 */
extern const bear_cfft_instance_q15 bear_cfft_inst_q15_len16;

/**
 * @brief  Pre-initialized constant instance for the 32 pt Q15 CFFT/CIFFT
 */
extern const bear_cfft_instance_q15 bear_cfft_inst_q15_len32;

/**
 * @brief  Pre-initialized constant instance for the 64 pt Q15 CFFT/CIFFT
 */
extern const bear_cfft_instance_q15 bear_cfft_inst_q15_len64;

/**
 * @brief  Pre-initialized constant instance for the 128 pt Q15 CFFT/CIFFT
 */
extern const bear_cfft_instance_q15 bear_cfft_inst_q15_len128;

/**
 * @brief  Pre-initialized constant instance for the 256 pt Q15 CFFT/CIFFT
 */
extern const bear_cfft_instance_q15 bear_cfft_inst_q15_len256;

/**
 * @brief  Pre-initialized constant instance for the 512 pt Q15 CFFT/CIFFT
 */
extern const bear_cfft_instance_q15 bear_cfft_inst_q15_len512;

/**
 * @brief  Pre-initialized constant instance for the 1024 pt Q15 CFFT/CIFFT
 */
extern const bear_cfft_instance_q15 bear_cfft_inst_q15_len1024;

/**
 * @brief  Pre-initialized constant instance for the 2048 pt Q15 CFFT/CIFFT
 */
extern const bear_cfft_instance_q15 bear_cfft_inst_q15_len2048;

/**
 * @brief  Pre-initialized constant instance for the 4096 pt Q15 CFFT/CIFFT
 */
extern const bear_cfft_instance_q15 bear_cfft_inst_q15_len4096;

/**
 * @brief         Get a pointer to an appropriate CFFT constant instance
 * @param[in]     fft_len   FFT length (number of complex samples)
 * @return        A pointer to a const bear_cfft_instance_q15 for requested
 *                fft_len or NULL if the fft_len isn't supported.
 */
const bear_cfft_instance_q15* bear_cfft_get_inst_q15(uint16_t fft_len);

/**
 * @brief         In-Place processing function for Q15 complex FFT.
 * @param[in]     inst        points to an instance of Q15 CFFT structure
 * @param[in,out] data        points to the complex data buffer of size
 *                            2*fft_len. Processing occurs in-place
 * @param[in]     ifft_flag   flag that selects transform direction
 *                               0: forward transform
 *                               1: inverse transform
 * @param[in]     bitrev_flag flag that enables / disables bit reversal of
 *                            output
 *                               0: disables bit reversal of output
 *                               1: enables bit reversal of output
 * @return        none
 */
void bear_cfft_q15(const bear_cfft_instance_q15* inst, q15_t* data,
                   uint8_t ifft_flag, uint8_t bitrev_flag);

/**
 * @brief Instance structure for the fixed-point CFFT/CIFFT function.
 */
typedef struct {
    const q31_t* twiddle_table;   /**< A pointer to the twiddle factor table */
    const uint16_t* bitrev_table; /**< A pointer to the bit reversal table */
    uint16_t fft_len;             /**< FFT length */
    uint16_t bitrev_len;          /**< Bit reversal table length. */
} bear_cfft_instance_q31;

/**
 * @brief  Pre-initialized constant instance for the 16 pt Q31 CFFT/CIFFT
 */
extern const bear_cfft_instance_q31 bear_cfft_inst_q31_len16;

/**
 * @brief  Pre-initialized constant instance for the 32 pt Q31 CFFT/CIFFT
 */
extern const bear_cfft_instance_q31 bear_cfft_inst_q31_len32;

/**
 * @brief  Pre-initialized constant instance for the 64 pt Q31 CFFT/CIFFT
 */
extern const bear_cfft_instance_q31 bear_cfft_inst_q31_len64;

/**
 * @brief  Pre-initialized constant instance for the 128 pt Q31 CFFT/CIFFT
 */
extern const bear_cfft_instance_q31 bear_cfft_inst_q31_len128;

/**
 * @brief  Pre-initialized constant instance for the 256 pt Q31 CFFT/CIFFT
 */
extern const bear_cfft_instance_q31 bear_cfft_inst_q31_len256;

/**
 * @brief  Pre-initialized constant instance for the 512 pt Q31 CFFT/CIFFT
 */
extern const bear_cfft_instance_q31 bear_cfft_inst_q31_len512;

/**
 * @brief  Pre-initialized constant instance for the 1024 pt Q31 CFFT/CIFFT
 */
extern const bear_cfft_instance_q31 bear_cfft_inst_q31_len1024;

/**
 * @brief  Pre-initialized constant instance for the 2048 pt Q31 CFFT/CIFFT
 */
extern const bear_cfft_instance_q31 bear_cfft_inst_q31_len2048;

/**
 * @brief  Pre-initialized constant instance for the 4096 pt Q31 CFFT/CIFFT
 */
extern const bear_cfft_instance_q31 bear_cfft_inst_q31_len4096;

/**
 * @brief         Processing function for the Q31 complex FFT.
 * @param[in]     inst        points to an instance of Q31 CFFT structure
 * @param[in,out] data        points to the complex data buffer of size
 *                            2*fft_len. Processing occurs in-place
 * @param[in]     ifft_flag   flag that selects transform direction
 *                              0: forward transform
 *                              1: inverse transform
 * @param[in]     bitrev_flag flag that enables / disables bit reversal of
 *                            output
 *                               0: disables bit reversal of output
 *                               1: enables bit reversal of output
 * @return        none
 */
void bear_cfft_q31(const bear_cfft_instance_q31* inst, q31_t* data,
                   uint8_t ifft_flag, uint8_t bitrev_flag);

/**
 * @brief         Get a pointer to an appropriate Q31 CFFT constant instance
 * @param[in]     fft_len   FFT length (number of complex samples)
 * @return        A pointer to a const bear_cfft_instance_q31 for requested
 *                fft_len or NULL if the fft_len isn't supported.
 */
const bear_cfft_instance_q31* bear_cfft_get_inst_q31(uint16_t fft_len);

/**
 *  @brief Instance structure for the Q15 RFFT/RIFFT function.
 */
typedef struct {
    const bear_cfft_instance_q15* cfft_inst; /**< points to the complex FFT
                                                  instance. */
    const q15_t* twiddle_table_a;  /**< points to the real twiddle factor
                                        table. */
    const q15_t* twiddle_table_b;  /**< points to the imag twiddle factor
                                        table. */
    uint32_t fft_len;        /**< length of the real FFT. */
    uint32_t twid_coeff_mod; /**< twiddle coefficient modifier that supports
                                 different size FFTs with the same twiddle
                                 factor table. */
    uint8_t ifft_flag;       /**< flag that selects forward (ifftFlagR=0)
                                  or inverse (ifftFlagR=1) transform. */
    uint8_t bitrev_flag;     /**< flag that enables (bitReverseFlagR=1) or
                                  disables (bitReverseFlagR=0) bit reversal
                                  of output. */
} bear_rfft_instance_q15;

/**
 * @brief         Generic initialization function for the Q15 RFFT/RIFFT.
 * @param[in,out] inst     points to an instance of the Q15 RFFT/RIFFT structure
 * @param[in]     fft_len  length of the FFT
 * @param[in]     ifft_flag flag that selects transform direction
 *                  0: forward transform
 *                  1: inverse transform
 * @param[in]     bitrev_flag flag that enables / disables bit reversal of output
 *                  0: disables bit reversal of output
 *                  1: enables bit reversal of output
 * @return        execution status
 *                  BEAR_MATH_SUCCESS        : Operation successful
 *                  BEAR_MATH_ARGUMENT_ERROR : fft_len is not a supported length
 */
bear_status_t bear_rfft_init_q15(bear_rfft_instance_q15* inst, uint32_t fft_len,
                                 uint32_t ifft_flag, uint32_t bitrev_flag);

/**
 * @brief         Initialization function for the 32 pt Q15 real FFT.
 * @details
 *       See bear_rfft_init_q15 for the information on the similar parameters.
 */
bear_status_t bear_rfft_init_32_q15(bear_rfft_instance_q15* inst,
                                    uint32_t ifft_flag, uint32_t bitrev_flag);

/**
 * @brief         Initialization function for the 64 pt Q15 real FFT.
 * @details
 *       See bear_rfft_init_q15 for the information on the similar parameters.
 */
bear_status_t bear_rfft_init_64_q15(bear_rfft_instance_q15* inst,
                                    uint32_t ifft_flag, uint32_t bitrev_flag);

/**
 * @brief         Initialization function for the 128 pt Q15 real FFT.
 * @details
 *       See bear_rfft_init_q15 for the information on the similar parameters.
 */
bear_status_t bear_rfft_init_128_q15(bear_rfft_instance_q15* inst,
                                     uint32_t ifft_flag, uint32_t bitrev_flag);

/**
 * @brief         Initialization function for the 256 pt Q15 real FFT.
 * @details
 *       See bear_rfft_init_q15 for the information on the similar parameters.
 */
bear_status_t bear_rfft_init_256_q15(bear_rfft_instance_q15* inst,
                                     uint32_t ifft_flag, uint32_t bitrev_flag);

/**
 * @brief         Initialization function for the 512 pt Q15 real FFT.
 * @details
 *       See bear_rfft_init_q15 for the information on the similar parameters.
 */
bear_status_t bear_rfft_init_512_q15(bear_rfft_instance_q15* inst,
                                     uint32_t ifft_flag, uint32_t bitrev_flag);

/**
 * @brief         Initialization function for the 1024 pt Q15 real FFT.
 * @details
 *       See bear_rfft_init_q15 for the information on the similar parameters.
 */
bear_status_t bear_rfft_init_1024_q15(bear_rfft_instance_q15* inst,
                                      uint32_t ifft_flag, uint32_t bitrev_flag);

/**
 * @brief         Initialization function for the 2048 pt Q15 real FFT.
 * @details
 *       See bear_rfft_init_q15 for the information on the similar parameters.
 */
bear_status_t bear_rfft_init_2048_q15(bear_rfft_instance_q15* inst,
                                      uint32_t ifft_flag, uint32_t bitrev_flag);

/**
 * @brief         Initialization function for the 4096 pt Q15 real FFT.
 * @details
 *       See bear_rfft_init_q15 for the information on the similar parameters.
 */
bear_status_t bear_rfft_init_4096_q15(bear_rfft_instance_q15* inst,
                                      uint32_t ifft_flag, uint32_t bitrev_flag);

/**
 * @brief         Initialization function for the 8192 pt Q15 real FFT.
 * @details
 *       See bear_rfft_init_q15 for the information on the similar parameters.
 */
bear_status_t bear_rfft_init_8192_q15(bear_rfft_instance_q15* inst,
                                      uint32_t ifft_flag, uint32_t bitrev_flag);

/**
 * @brief         Processing function for the Q15 RFFT/RIFFT.
 * @param[in]     inst  points to an instance of the Q15 RFFT/RIFFT structure
 * @param[in]     src  points to input buffer (Source buffer is modified by this
 *                     function).
 * @param[out]    dst  points to output buffer
 * @return        none
 *
 * @details  For more information on function see the BEAR-DSP Documentation.
 */
void bear_rfft_q15(const bear_rfft_instance_q15* inst, q15_t* src, q15_t* dst);

/**
 * @brief Instance structure for the Q31 RFFT/RIFFT function.
 */
typedef struct {
    const bear_cfft_instance_q31* cfft_inst; /**< points to the complex FFT
                                                  instance. */
    const q31_t* twiddle_table_a; /**< points to the real twiddle factor
                                       table. */
    const q31_t* twiddle_table_b; /**< points to the imag twiddle factor
                                       table. */
    uint32_t fft_len;             /**< length of the real FFT. */
    uint32_t twid_coeff_mod; /**< twiddle coefficient modifier that supports
                                  different size FFTs with the same twiddle
                                  factor table. */
    uint8_t ifft_flag;   /**< flag that selects forward (ifftFlagR=0) or inverse
                              (ifftFlagR=1) transform. */
    uint8_t bitrev_flag; /**< flag that enables (bitReverseFlagR=1) or disables
                              (bitReverseFlagR=0) bit reversal of output. */
} bear_rfft_instance_q31;

/**
 * @brief         Generic initialization function for the Q31 RFFT/RIFFT.
 * @param[in,out] inst   points to an instance of the Q31 RFFT/RIFFT structure
 * @param[in]     fft_len    length of the FFT
 * @param[in]     ifft_flag  flag that selects transform direction
 *                  0: forward transform
 *                  1: inverse transform
 * @param[in]     bitrev_flag flag that enables / disables bit reversal of output
 *                  0: disables bit reversal of output
 *                  1: enables bit reversal of output
 * @return        execution status
 *                  BEAR_MATH_SUCCESS        : Operation successful
 *                  BEAR_MATH_ARGUMENT_ERROR : fft_len is not a supported length
 */
bear_status_t bear_rfft_init_q31(bear_rfft_instance_q31* inst, uint32_t fft_len,
                                 uint32_t ifft_flag, uint32_t bitrev_flag);

/**
 * @brief         Initialization function for the 32 pt Q31 real FFT.
 * @details
 *       See bear_rfft_init_q31 for the information on the similar parameters.
 */
bear_status_t bear_rfft_init_32_q31(bear_rfft_instance_q31* inst,
                                    uint32_t ifft_flag, uint32_t bitrev_flag);

/**
 * @brief         Initialization function for the 64 pt Q31 real FFT.
 * @details
 *       See bear_rfft_init_q31 for the information on the similar parameters.
 */
bear_status_t bear_rfft_init_64_q31(bear_rfft_instance_q31* inst,
                                    uint32_t ifft_flag, uint32_t bitrev_flag);

/**
 * @brief         Initialization function for the 128 pt Q31 real FFT.
 * @details
 *       See bear_rfft_init_q31 for the information on the similar parameters.
 */
bear_status_t bear_rfft_init_128_q31(bear_rfft_instance_q31* inst,
                                     uint32_t ifft_flag, uint32_t bitrev_flag);

/**
 * @brief         Initialization function for the 256 pt Q31 real FFT.
 * @details
 *       See bear_rfft_init_q31 for the information on the similar parameters.
 */
bear_status_t bear_rfft_init_256_q31(bear_rfft_instance_q31* inst,
                                     uint32_t ifft_flag, uint32_t bitrev_flag);

/**
 * @brief         Initialization function for the 512 pt Q31 real FFT.
 * @details
 *       See bear_rfft_init_q31 for the information on the similar parameters.
 */
bear_status_t bear_rfft_init_512_q31(bear_rfft_instance_q31* inst,
                                     uint32_t ifft_flag, uint32_t bitrev_flag);

/**
 * @brief         Initialization function for the 1024 pt Q31 real FFT.
 * @details
 *       See bear_rfft_init_q31 for the information on the similar parameters.
 */
bear_status_t bear_rfft_init_1024_q31(bear_rfft_instance_q31* inst,
                                      uint32_t ifft_flag, uint32_t bitrev_flag);

/**
 * @brief         Initialization function for the 2048 pt Q31 real FFT.
 * @details
 *       See bear_rfft_init_q31 for the information on the similar parameters.
 */
bear_status_t bear_rfft_init_2048_q31(bear_rfft_instance_q31* inst,
                                      uint32_t ifft_flag, uint32_t bitrev_flag);

/**
 * @brief         Initialization function for the 4096 pt Q31 real FFT.
 * @details
 *       See bear_rfft_init_q31 for the information on the similar parameters.
 */
bear_status_t bear_rfft_init_4096_q31(bear_rfft_instance_q31* inst,
                                      uint32_t ifft_flag, uint32_t bitrev_flag);

/**
 * @brief         Initialization function for the 8192 pt Q31 real FFT.
 * @details
 *       See bear_rfft_init_q31 for the information on the similar parameters.
 */
bear_status_t bear_rfft_init_8192_q31(bear_rfft_instance_q31* inst,
                                      uint32_t ifft_flag, uint32_t bitrev_flag);

/**
 * @brief         Processing function for the Q31 RFFT/RIFFT.
 * @param[in]     inst points to an instance of the Q31 RFFT/RIFFT structure
 * @param[in]     src  points to input buffer (Source buffer is modified by this
 *                     function)
 * @param[out]    dst  points to output buffer
 * @return        none
 *
 * @details  For more information on function see the BEAR-DSP Documentation.
 */
void bear_rfft_q31(const bear_rfft_instance_q31* inst, q31_t* src, q31_t* dst);

/**
 * @brief Instance structure for the Q15 MFCC function.
 */
typedef struct {
    const q15_t* dct_coeff;             /**< Internal DCT coefficients */
    const q15_t* filter_coeff;          /**< Internal Mel filter coefficients*/
    const q15_t* window_coeff;          /**< Windowing coefficients */
    const uint32_t* filter_pos;         /**< Internal Mel filter positions
                                             in spectrum */
    const uint32_t* filter_len;         /**< Internal Mel filter  lengths */
    bear_rfft_instance_q15 rfft_inst;   /**< points to the Real FFT
                                             instance. */
    uint32_t fft_len;                   /**< FFT length */
    uint32_t nb_mel_filtrs;             /**< Number of Mel filters */
    uint32_t nb_dct_out;                /**< Number of DCT outputs */
} bear_mfcc_instance_q15;

/**
  @brief         Generic initialization of the MFCC Q15 instance structure
  @param[out]    inst           points to the mfcc instance structure
  @param[in]     fft_len        fft length
  @param[in]     nb_mel_filtrs  number of Mel filters
  @param[in]     nb_dct_out     number of Dct outputs
  @param[in]     dct_coeff      points to an array of DCT coefficients
  @param[in]     filter_pos     points of the array of filter positions
  @param[in]     filter_len     points to the array of filter lengths
  @param[in]     filter_coeff   points to the array of filter coefficients
  @param[in]     window_coeff   points to the array of window coefficients

  @return        error status

  @par          Description
                The matrix of Mel filter coefficients is sparse.
                Most of the coefficients are zero.
                To avoid multiplying the spectrogram by those zeros, the
                filter is applied only to a given position in the spectrogram
                and on a given number of FFT bins (the filter length).
                It is the reason for the arrays filterPos and filterLengths.

                window coefficients can describe (for instance) a Hamming
                window. The array has the same size as the FFT length.

                The folder Scripts is containing a Python script which can be
                used to generate the filter, dct and window arrays.

  @par
                This function should be used only if you don't know the FFT
                sizes that you'll need at build time. The use of this function
                will prevent the linker from removing the FFT tables that are
                not needed and the library code size will be bigger than needed.

  @par
                If you use CMSIS-DSP as a static library, and if you know the
                MFCC sizes that you need at build time, then it is better to use
                the initialization functions defined for each MFCC size.
 */
bear_status_t bear_mfcc_init_q15(bear_mfcc_instance_q15* inst, uint32_t fft_len,
                                 uint32_t nb_mel_filtrs, uint32_t nb_dct_out,
                                 const q15_t* dct_coeff,
                                 const uint32_t* filter_pos,
                                 const uint32_t* filter_len,
                                 const q15_t* filter_coeff,
                                 const q15_t* window_coeff);

/**
  @brief  Initialization function for the 32 pt Q15 MFCC.
  @par    See bear_mfcc_init_q15 for the information on the similar parameters.
 */
bear_status_t bear_mfcc_init_32_q15(bear_mfcc_instance_q15* inst,
                                    uint32_t nb_mel_filtrs, uint32_t nb_dct_out,
                                    const q15_t* dct_coeff,
                                    const uint32_t* filter_pos,
                                    const uint32_t* filter_len,
                                    const q15_t* filter_coeff,
                                    const q15_t* window_coeff);

/**
  @brief  Initialization function for the 64 pt Q15 MFCC.
  @par    See bear_mfcc_init_q15 for the information on the similar parameters.
 */
bear_status_t bear_mfcc_init_64_q15(bear_mfcc_instance_q15* inst,
                                    uint32_t nb_mel_filtrs, uint32_t nb_dct_out,
                                    const q15_t* dct_coeff,
                                    const uint32_t* filter_pos,
                                    const uint32_t* filter_len,
                                    const q15_t* filter_coeff,
                                    const q15_t* window_coeff);

/**
  @brief  Initialization function for the 128 pt Q15 MFCC.
  @par    See bear_mfcc_init_q15 for the information on the similar parameters.
 */
bear_status_t bear_mfcc_init_128_q15(
    bear_mfcc_instance_q15* inst, uint32_t nb_mel_filtrs, uint32_t nb_dct_out,
    const q15_t* dct_coeff, const uint32_t* filter_pos,
    const uint32_t* filter_len, const q15_t* filter_coeff,
    const q15_t* window_coeff);

/**
  @brief  Initialization function for the 256 pt Q15 MFCC.
  @par    See bear_mfcc_init_q15 for the information on the similar parameters.
 */
bear_status_t bear_mfcc_init_256_q15(
    bear_mfcc_instance_q15* inst, uint32_t nb_mel_filtrs, uint32_t nb_dct_out,
    const q15_t* dct_coeff, const uint32_t* filter_pos,
    const uint32_t* filter_len, const q15_t* filter_coeff,
    const q15_t* window_coeff);

/**
  @brief  Initialization function for the 512 pt Q15 MFCC.
  @par    See bear_mfcc_init_q15 for the information on the similar parameters.
 */
bear_status_t bear_mfcc_init_512_q15(
    bear_mfcc_instance_q15* inst, uint32_t nb_mel_filtrs, uint32_t nb_dct_out,
    const q15_t* dct_coeff, const uint32_t* filter_pos,
    const uint32_t* filter_len, const q15_t* filter_coeff,
    const q15_t* window_coeff);

/**
  @brief  Initialization function for the 1024 pt Q15 MFCC.
  @par    See bear_mfcc_init_q15 for the information on the similar parameters.
 */
bear_status_t bear_mfcc_init_1024_q15(
    bear_mfcc_instance_q15* inst, uint32_t nb_mel_filtrs, uint32_t nb_dct_out,
    const q15_t* dct_coeff, const uint32_t* filter_pos,
    const uint32_t* filter_len, const q15_t* filter_coeff,
    const q15_t* window_coeff);

/**
  @brief  Initialization function for the 2048 pt Q15 MFCC.
  @par    See bear_mfcc_init_q15 for the information on the similar parameters.
 */
bear_status_t bear_mfcc_init_2048_q15(
    bear_mfcc_instance_q15* inst, uint32_t nb_mel_filtrs, uint32_t nb_dct_out,
    const q15_t* dct_coeff, const uint32_t* filter_pos,
    const uint32_t* filter_len, const q15_t* filter_coeff,
    const q15_t* window_coeff);

/**
  @brief  Initialization function for the 4096 pt Q15 MFCC.
  @par    See bear_mfcc_init_q15 for the information on the similar parameters.
 */
bear_status_t bear_mfcc_init_4096_q15(
    bear_mfcc_instance_q15* inst, uint32_t nb_mel_filtrs, uint32_t nb_dct_out,
    const q15_t* dct_coeff, const uint32_t* filter_pos,
    const uint32_t* filter_len, const q15_t* filter_coeff,
    const q15_t* window_coeff);

/**
  @brief         MFCC Q15
  @param[in]     inst  points to the mfcc instance structure
  @param[in]     src   points to the input samples
  @param[out]    dst   points to the output MFCC values in q8.7 format
  @param[inout]  tmp   points to a temporary buffer of complex
  @return        error status
 */
bear_status_t bear_mfcc_q15(const bear_mfcc_instance_q15* inst, q15_t* src,
                            q15_t* dst, q31_t* tmp);

#ifdef   __cplusplus
}
#endif

#endif /* ifndef _TRANSFORM_FUNCTIONS_H_ */
