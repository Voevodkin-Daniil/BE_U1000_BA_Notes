/*****************************************************************************
 * @brief Public header with platform and compiler specific code definitions.
 *
 *****************************************************************************
 * Copyright (c) 2022 CloudBEAR LLC, all rights reserved.
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
 ******************************************************************************/

#ifndef _BEAR_DSP_CONFIG_H
#define _BEAR_DSP_CONFIG_H


/*******************************************************************
 * Target platform configuration
 ******************************************************************/
/** \brief Determine a targett platform and available extensions*/
#if defined(__riscv_p) && !defined(PLATFORM_GENERIC)
// RISC-V with P (Packed SIMD) extension.
#  define PLATFORM_RISCV
#  define RISCV_P_SIMD_EXT

#elif !defined(PLATFORM_GENERIC)
// Platform w/o any specific extensions. Targeting platform agnostic code.
#  define PLATFORM_GENERIC
#endif

/** \brief Define platform xlen in the consistent form with other defines. */
#define RV64_XLEN (64)
#define RV32_XLEN (32)
#if defined(__riscv_xlen)
#  if (__riscv_xlen == RV64_XLEN || __riscv_xlen == RV32_XLEN)
#    define RISCV_XLEN __riscv_xlen
#  else
#    error "Unsupported XLEN configuration of the target"
#  endif
#else
#  define RISCV_XLEN RV32_XLEN
#endif /* #if defined(__riscv_xlen) */

/*******************************************************************
 * Implementation configuration
 ******************************************************************/
// Compile pure CMSIS-DSP reference code w/o BEAR tweaks introduced during
// RISCV port process.
// #define CMSIS_PURE_REFERENCE

// Use rounding up cast of multiplication result in fixed point CFFT functions
// (affects RISCV_P_SIMD_EXT port only).
// #define CFFT_FX_ROUND_UP

// Use 64bit accumulator for multiplication result instead of immediate
// result cast to 32bit. This make results aligned with RV64 arch version.
// Affects only RISCV_P_SIMD_EXT port of cfft_q31 for RV32 arch
// #define CFFT_Q31_RV32_FULL_ACC

/*******************************************************************
 * Code attributes
 ******************************************************************/
/** \brief Define a static function that may be inlined by the compiler. */
#define __STATIC_INLINE static inline

/** \brief Define a static function that should be always inlined by the
 * compiler. */
#define __STATIC_FORCEINLINE __attribute__((always_inline)) static inline

/** \brief Minimum `x` bytes alignment for a variable. */
#define __ALIGNED(x) __attribute__((aligned(x)))

/** \brief Minimum register bytes depth alignment for a data. */
#define __ARCH_DATA_ALIGNMENT __ALIGNED(RISCV_XLEN / 8)

#endif /* _BEAR_DSP_CONFIG_H */
