/*****************************************************************************
 * @brief Declaration of all CMSIS-DSP compatible controller functions
 *
 *****************************************************************************
 * Copyright (c) 2010-2020 Arm Limited or its affiliates. All rights reserved.
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
 *****************************************************************************/

#ifndef _CONTROLLER_FUNCTIONS_H_
#define _CONTROLLER_FUNCTIONS_H_

#include "bear_dsp_config.h"
#include "bear_dsp_math_types.h"
#include "intrinsics/rvintrin.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
  @defgroup SinCos Sine Cosine

  Computes the trigonometric sine and cosine values using a combination of table
  lookup and linear interpolation. There are separate functions for Q31 and
  floating-point data types. The input to the floating-point version is in
  degrees while the fixed-point Q31 have a scaled input with the range
  [-1 0.9999] mapping to [-180 +180] degrees.

  The floating point function also allows values that are out of the usual
  range. When this happens, the function will take extra time to adjust the
  input value to the range of [-180 180].

  The result is accurate to 5 digits after the decimal point.

  The implementation is based on table lookup using 360 values together with
  linear interpolation. The steps used are:
   -# Calculation of the nearest integer table index.
   -# Compute the fractional portion (fract) of the input.
   -# Fetch the value corresponding to \c index from sine table to \c y0 and
  also value from \c index+1 to \c y1.
   -# Sine value is computed as <code> sin_val = y0 + (fract * (y1 -
  y0))</code>.
   -# Fetch the value corresponding to \c index from cosine table to \c y0 and
  also value from \c index+1 to \c y1.
   -# Cosine value is computed as <code> cos_val = y0 + (fract * (y1 -
  y0))</code>.
 */

/**
 * @brief Number of fractional bits of the theta input parameter
 * in bear_sin_cos_q31 functions
 */
#define SINCOS_THETA_Q31_FRAQ_BITS (23)

/**
 * @brief  Q31 sin_cos function.
 * @param[in]  theta    scaled input value in degrees
 * @param[out] sin_val  points to the processed sine output.
 * @param[out] cos_val  points to the processed cosine output.
 */
void bear_sin_cos_q31(q31_t theta, q31_t* sin_val, q31_t* cos_val);


/**
 * @brief  Floating-point sin_cos function.
 * @param[in]  theta   input value in degrees
 * @param[out] sin_val  points to the processed sine output.
 * @param[out] cos_val  points to the processed cos output.
 */
void bear_sin_cos_f32(float32_t theta, float32_t* sin_val, float32_t* cos_val);

/**
 * @brief Instance structure for the Q15 PID Control.
 */
typedef struct {
    q15_t Kp;       /**< The proportional gain. */
    q15_t Ki;       /**< The integral gain. */
    q15_t Kd;       /**< The derivative gain. */

    union pid_interntal_q15 {
        struct {
            v2int16_t negone_A0;   /**< -1 constant and A0 derived gain vector. */
            v2int16_t A1_A2;       /**< A1 and A2  derived gains vector. */
            v2int16_t x1_x2;       /**< Two previous samples vector. */
            int16_t y1;            /**< Previous output state */
        } rv32;

        struct {
            v4int16_t A2_A1_A0_negone;   /**< The derived gains vector <A2, A1, A0, -1> */
            v4int16_t x2_x1_y1;          /**< State vector <x(t-2), x(t-1), undef_val, y(t-1)> */
        } rv64;
    } state;
} bear_pid_instance_q15;

/**
 * @brief Instance structure for the Q31 PID Control.
 */
typedef struct {
    q31_t Kp;       /**< The proportional gain. */
    q31_t Ki;       /**< The integral gain. */
    q31_t Kd;       /**< The derivative gain. */

    union pid_interntal_q31 {
        struct {
            q31_t A0;          /**< The derived gain, A0 = Kp + Ki + Kd . */
            q31_t A1;          /**< The derived gain, A1 = -Kp - 2Kd. */
            q31_t A2;          /**< The derived gain, A2 = Kd . */
            q31_t x1;          /**< The sample of (t-1) iteration */
            q31_t x2;          /**< The sample of (t-2) iteration */
            q31_t y1;          /**< Previous output state */
        } rv32;

        struct {
            v2int32_t negone_A0;   /**< -1 constant and A0 derived gain vector. */
            v2int32_t A1_A2;       /**< A1 and A2  derived gains vector. */
            v2int32_t x1_x2;       /**< Two previous samples vector. */
            int32_t y1;            /**< Previous output state */
        } rv64;
    } state;                       /**< Internal platform specific state union */
} bear_pid_instance_q31;

/**
 * @brief Instance structure for the floating-point PID Control.
 */
typedef struct {
    float32_t A0;       /**< The derived gain, A0 = Kp + Ki + Kd . */
    float32_t A1;       /**< The derived gain, A1 = -Kp - 2Kd. */
    float32_t A2;       /**< The derived gain, A2 = Kd . */
    float32_t state[3]; /**< The state array of length 3. */
    float32_t Kp;       /**< The proportional gain. */
    float32_t Ki;       /**< The integral gain. */
    float32_t Kd;       /**< The derivative gain. */
} bear_pid_instance_f32;

/**
 * @brief  Initialization function for the FP32 PID controller instance
 * @param[in,out] instance          Pointer to an instance of the PID controller
 * @param[in]     reset_state_flag  Reset the instance’s state flag
 *                                      0 = Preserve the state of the instance
 *                                      1 = Set the instance state to zero
 */
void bear_pid_init_f32(bear_pid_instance_f32* instance,
                       int32_t reset_state_flag);

/**
 * @brief  Function to reset the state of the FP32 PID controller instance
 * @param[in,out] instance  is an instance of the floating-point PID Control
 *                          structure
 */
void bear_pid_reset_f32(bear_pid_instance_f32* instance);

/**
 * @brief  Initialization function for the Q31 PID controller instance
 * @param[in,out] instance          Pointer to an instance of the PID controller
 * @param[in]     reset_state_flag  Reset the instance’s state flag
 *                                      0 = Preserve the state of the instance
 *                                      1 = Set the instance state to zero
 */
void bear_pid_init_q31(bear_pid_instance_q31* instance,
                       int32_t reset_state_flag);

/**
 * @brief  Function to reset the state of the Q31 PID controller instance
 * @param[in,out] instance   Pointer to an instance of the PID controller
 *                           structure
 */
void bear_pid_reset_q31(bear_pid_instance_q31* instance);

/**
 * @brief  Initialization function for the Q15 PID controller instance
 * @param[in,out] instance          Pointer to an instance of the PID controller
 * @param[in]     reset_state_flag  Reset the instance’s state flag
 *                                      0 = Preserve the state of the instance
 *                                      1 = Set the instance state to zero
 */
void bear_pid_init_q15(bear_pid_instance_q15* instance,
                       int32_t reset_state_flag);

/**
 * @brief  Function to reset the state of the Q15 PID controller instance
 * @param[in,out] instance  Pointer to an instance of the PID controller
 */
void bear_pid_reset_q15(bear_pid_instance_q15* instance);

/**
 * @brief         Processing function for the FP32 PID controller
 * @param[in,out] inst    Pointer to an instance of the PID controller
 * @param[in]     sample  Current input sample to process
 * @return        Output PID correction value
 */
float32_t bear_pid_f32(bear_pid_instance_f32* inst, float32_t sample);

/**
  @brief         Processing function for the Q31 PID controller
  @param[in,out] inst    Pointer to an instance of the PID controller
  @param[in]     sample  Current input sample to process
  @return        Output PID correction value
 */
#if defined(PLATFORM_GENERIC)
q31_t bear_pid_q31(bear_pid_instance_q31* inst, q31_t sample);

#else  // RISCV_P_SIMD_EXT
__STATIC_FORCEINLINE q31_t bear_pid_q31(bear_pid_instance_q31* inst,
                                        q31_t sample)
{
#  if (RISCV_XLEN == RV32_XLEN)
    const unsigned long frac_bits = 31;
    const q31_t neg_one_q31 = 0x80000000;
    q63_t acc = 1 << (frac_bits - 1);

    // acc = (A0 * sample) + (A1 * x1) + (A2 * x2) - (-1 * y1)
    acc = __rv_smsr64(acc, neg_one_q31, inst->state.rv32.y1);
    acc = __rv_smar64(acc, inst->state.rv32.A0, sample);
    acc = __rv_smar64(acc, inst->state.rv32.A1, inst->state.rv32.x1);
    acc = __rv_smar64(acc, inst->state.rv32.A2, inst->state.rv32.x2);

    // out = acc >> frac_bits
    const q31_t out =
        __rv_fsr((uint32_t)acc, (frac_bits), (uint32_t)(acc >> 32));

    // Update state
    inst->state.rv32.x2 = inst->state.rv32.x1;
    inst->state.rv32.x1 = sample;
    inst->state.rv32.y1 = out;
    return out;

#  else
    // concat(y1, x0)
    v2int32_t temp = __rv_pkbb32(inst->state.rv64.y1, sample);

    // acc = (A0 * x0) - (y1 * (-1))
    q63_t acc = __rv_smdrs32(inst->state.rv64.negone_A0, temp);

    // concat(x0,x1) for next iteration
    temp = __rv_pkbt32(temp, inst->state.rv64.x1_x2);

    // acc += (A1 * x1) + (A2 * x2)
    acc = __rv_smar64(acc, inst->state.rv64.A1_A2, inst->state.rv64.x1_x2);
    inst->state.rv64.x1_x2 = temp;

    const unsigned long frac_bits = 31;
    const q31_t out = (q31_t)__rv_sra_u(acc, frac_bits);
    inst->state.rv64.y1 = out;
    return out;

#  endif
}
#endif // #if defined(PLATFORM_GENERIC)

/**
  @brief         Processing function for the Q15 PID controller
  @param[in,out] inst   Pointer to an instance of the PID controller
  @param[in]     sample Current input sample to process
  @return        Output PID correction value
 */
#if defined(PLATFORM_GENERIC)
q15_t bear_pid_q15(bear_pid_instance_q15* inst, q15_t sample);

#else  // RISCV_P_SIMD_EXT
__STATIC_FORCEINLINE q15_t bear_pid_q15(bear_pid_instance_q15* inst,
                                        q15_t sample)
{
#  if (RISCV_XLEN == RV32_XLEN)
    // concat(y1, x0)
    v2int16_t temp = __rv_pkbb16(inst->state.rv32.y1, sample);

    // acc = (A0 * x0) - (y1 * (-1))
    q31_t acc = __rv_smdrs(inst->state.rv32.negone_A0, temp);

    // concat(x0,x1) for next iteration
    temp = __rv_pkbt16(temp, inst->state.rv32.x1_x2);

    // acc += (A1 * x1) + (A2 * x2)
    acc = __rv_kmada(acc, inst->state.rv32.A1_A2, inst->state.rv32.x1_x2);
    inst->state.rv32.x1_x2 = temp;

    const unsigned long frac_bits = 15;
    acc = __rv_sra_u(acc, frac_bits);
    const q15_t out = (q15_t)__rv_sclip32(acc, frac_bits);
    inst->state.rv32.y1 = out;
    return out;

#  else
    v4int16_t tmp_vector = inst->state.rv64.x2_x1_y1;
    const v4int16_t x0_y1 = __rv_pkbb16(sample, tmp_vector);
    const v4int16_t x2_x1_x0_y1 = __rv_pktb32(tmp_vector, x0_y1);

    // acc = - (-A2 * x2) - (-A1 * x1) -(-A0 * sample) - (-1 * y1)
    int64_t acc = 0;
    acc = __rv_smslda(acc, x2_x1_x0_y1, inst->state.rv64.A2_A1_A0_negone);

    // shift vector to have x2_x1 from x1_x0 for next iteration
    tmp_vector = x2_x1_x0_y1 << 16;

    const unsigned long frac_bits = 15;
    acc = __rv_sra_u(acc, frac_bits);
    acc = __rv_sclip32(acc, frac_bits);

    // make x2_x1_mock_y1 state
    inst->state.rv64.x2_x1_y1 = __rv_pktb32(tmp_vector, acc);
    const q15_t out = (q15_t)acc;
    return out;

#  endif
}
#endif  // #if defined(PLATFORM_GENERIC)

/**
 * @ingroup groupController
 */

/**
 * @defgroup park Vector Park Transform
 *
 * Forward Park transform converts the input two-coordinate vector to flux and
 * torque components. The Park transform can be used to realize the
 * transformation of the <code>I_alpha</code> and the <code>I_beta</code>
 * currents from the stationary to the moving reference frame and control the
 * spatial relationship between the stator vector current and rotor flux vector.
 * If we consider the d axis aligned with the rotor flux, the diagram below
 * shows the current vector and the relationship from the two reference frames:
 * \image html park.gif "Stator current space vector and its component in (a,b)
 * and in the d,q rotating reference frame"
 *
 * The function operates on a single sample of data and each call to the
 * function returns the processed output. The library provides separate
 * functions for Q31 and floating-point data types. \par Algorithm \image html
 * parkFormula.gif where <code>I_alpha</code> and <code>I_beta</code> are the
 * stator vector components, <code>I_d</code> and <code>I_q</code> are rotor
 * vector components and <code>cos_val</code> and <code>sin_val</code> are the
 * cosine and sine values of theta (rotor flux position).
 * \par Fixed-Point Behavior
 * Care must be taken when using the Q31 version of the Park transform.
 * In particular, the overflow and saturation behavior of the accumulator used
 * must be considered. Refer to the function specific documentation below for
 * usage guidelines.
 */

/**
  @ingroup park
  @brief Floating-point Park transform
  @param[in]  I_alpha  input two-phase vector coordinate alpha
  @param[in]  I_beta   input two-phase vector coordinate beta
  @param[out] I_d     points to output rotor reference frame d
  @param[out] I_q     points to output rotor reference frame q
  @param[in]  sin_val  sine value of rotation angle theta
  @param[in]  cos_val  cosine value of rotation angle theta
  @return     none

  The function implements the forward Park transform.

 */
void bear_park_f32(float32_t I_alpha, float32_t I_beta, float32_t* I_d,
                   float32_t* I_q, float32_t sin_val, float32_t cos_val);

/**
  @ingroup park
  @brief  Park transform for Q31 version
  @param[in]  I_alpha  input two-phase vector coordinate alpha
  @param[in]  I_beta   input two-phase vector coordinate beta
  @param[out] I_d     points to output rotor reference frame d
  @param[out] I_q     points to output rotor reference frame q
  @param[in]  sin_val  sine value of rotation angle theta
  @param[in]  cos_val  cosine value of rotation angle theta
  @return     none

  \par Scaling and Overflow Behavior
         The function is implemented using an internal 32-bit accumulator.
         The accumulator maintains 1.31 format by truncating lower 31 bits of
  the intermediate multiplication in 2.62 format. There is saturation on the
  addition and subtraction, hence there is no risk of overflow.
 */
#if defined(PLATFORM_GENERIC)
void bear_park_q31(q31_t I_alpha, q31_t I_beta, q31_t* I_d, q31_t* I_q,
                   q31_t sin_val, q31_t cos_val);
#else  // RISCV_P_SIMD_EXT
__STATIC_FORCEINLINE void bear_park_q31(q31_t I_alpha, q31_t I_beta, q31_t* I_d,
                                        q31_t* I_q, q31_t sin_val,
                                        q31_t cos_val)
{
    const q31_t product1 = __rv_kwmmul_u(I_alpha, cos_val);  // q31*q31 >31> q31
    const q31_t product2 = __rv_kwmmul_u(I_beta, sin_val);   // q31*q31 >31> q31
    const q31_t product3 = __rv_kwmmul_u(I_alpha, sin_val);  // q31*q31 >31> q31
    const q31_t product4 = __rv_kwmmul_u(I_beta, cos_val);   // q31*q31 >31> q31
    *I_d = __rv_kaddw(product1, product2);  // q31 + q31 = q31
    *I_q = __rv_ksubw(product4, product3);  // q31 - q31 = q31
}
#endif

/**
 * @ingroup groupController
 */

/**
 * @defgroup inv_park Vector Inverse Park transform
 * Inverse Park transform converts the input flux and torque components to
 * two-coordinate vector.
 *
 * The function operates on a single sample of data and each call to the
 * function returns the processed output. The library provides separate
 * functions for Q31 and floating-point data types. \par Algorithm \image html
 * parkInvFormula.gif where <code>I_alpha</code> and <code>I_beta</code> are the
 * stator vector components, <code>I_d</code> and <code>I_q</code> are rotor
 * vector components and <code>cos_val</code> and <code>sin_val</code> are the
 * cosine and sine values of theta (rotor flux position).
 * \par Fixed-Point Behavior
 * Care must be taken when using the Q31 version of the Park transform.
 * In particular, the overflow and saturation behavior of the accumulator used
 * must be considered. Refer to the function specific documentation below for
 * usage guidelines.
 */


/**
  @ingroup inv_park
  @brief  Floating-point Inverse Park transform
  @param[in]  I_d       input coordinate of rotor reference frame d
  @param[in]  I_q       input coordinate of rotor reference frame q
  @param[out] I_alpha  points to output two-phase orthogonal vector axis alpha
  @param[out] I_beta   points to output two-phase orthogonal vector axis beta
  @param[in]  sin_val   sine value of rotation angle theta
  @param[in]  cos_val   cosine value of rotation angle thetata
  @return     none
 */
void bear_inv_park_f32(float32_t I_d, float32_t I_q, float32_t* I_alpha,
                       float32_t* I_beta, float32_t sin_val, float32_t cos_val);

/**
  @ingroup inv_park
  @brief  Inverse Park transform for   Q31 version
  @param[in]  Id       input coordinate of rotor reference frame d
  @param[in]  Iq       input coordinate of rotor reference frame q
  @param[out] I_alpha  points to output two-phase orthogonal vector axis alpha
  @param[out] I_beta   points to output two-phase orthogonal vector axis beta
  @param[in]  sin_val   sine value of rotation angle theta
  @param[in]  cos_val   cosine value of rotation angle theta
  @return     none

  @par Scaling and Overflow Behavior
         The function is implemented using an internal 32-bit accumulator.
         The accumulator maintains 1.31 format by truncating lower 31 bits of
  the intermediate multiplication in 2.62 format. There is saturation on the
  addition, hence there is no risk of overflow.
 */
#if defined(PLATFORM_GENERIC)
void bear_inv_park_q31(q31_t I_d, q31_t I_q, q31_t* I_alpha, q31_t* I_beta,
                       q31_t sin_val, q31_t cos_val);
#else  // RISCV_P_SIMD_EXT
__STATIC_FORCEINLINE void bear_inv_park_q31(q31_t I_d, q31_t I_q,
                                            q31_t* I_alpha, q31_t* I_beta,
                                            q31_t sin_val, q31_t cos_val)
{
    const q31_t product1 = __rv_kwmmul_u(I_d, cos_val);  // q31*q31 >31> q31
    const q31_t product2 = __rv_kwmmul_u(I_q, sin_val);  // q31*q31 >31> q31
    const q31_t product3 = __rv_kwmmul_u(I_d, sin_val);  // q31*q31 >31> q31
    const q31_t product4 = __rv_kwmmul_u(I_q, cos_val);  // q31*q31 >31> q31
    *I_alpha = __rv_ksubw(product1, product2);
    *I_beta = __rv_kaddw(product3, product4);
}
#endif


/**
 * @ingroup groupController
 */

/**
 * @defgroup clarke Vector Clarke Transform
 * Forward Clarke transform converts the instantaneous stator phases into a
 * two-coordinate time invariant vector. Generally the Clarke transform uses
 * three-phase currents <code>I_a, I_b and Ic</code> to calculate currents
 * in the two-phase orthogonal stator axis <code>I_alpha</code> and
 * <code>I_beta</code>. When <code>I_alpha</code> is superposed with
 * <code>I_a</code> as shown in the figure below \image html clarke.gif
 * Stator current space vector and its components in (a,b). and <code>I_a +
 * I_b + Ic = 0</code>, in this condition <code>I_alpha</code> and
 * <code>I_beta</code> can be calculated using only <code>I_a</code> and
 * <code>I_b</code>.
 *
 * The function operates on a single sample of data and each call to the
 * function returns the processed output. The library provides separate
 * functions for Q31 and floating-point data types. \par Algorithm \image
 * html clarkeFormula.gif where <code>I_a</code> and <code>I_b</code> are
 * the instantaneous stator phases and <code>I_alpha</code> and
 * <code>I_beta</code> are the two coordinates of time invariant vector.
 * \par Fixed-Point Behavior Care must be taken when using the Q31 version
 * of the Clarke transform. In particular, the overflow and saturation
 * behavior of the accumulator used must be considered. Refer to the
 * function specific documentation below for usage guidelines.
 */

/**
  @ingroup clarke
  @brief  Floating-point Clarke transform
  @param[in]  I_a       input three-phase coordinate <code>a</code>
  @param[in]  I_b       input three-phase coordinate <code>b</code>
  @param[out] I_alpha  points to output two-phase orthogonal vector axis alpha
  @param[out] I_beta   points to output two-phase orthogonal vector axis beta
  @return        none
 */
void bear_clarke_f32(float32_t I_a, float32_t I_b, float32_t* I_alpha,
                     float32_t* I_beta);

/**
  @ingroup clarke
  @brief  Clarke transform for Q31 version
  @param[in]  I_a       input three-phase coordinate <code>a</code>
  @param[in]  I_b       input three-phase coordinate <code>b</code>
  @param[out] I_alpha  points to output two-phase orthogonal vector axis
  alpha
  @param[out] I_beta   points to output two-phase orthogonal vector axis
  beta
  @return     none

  \par Scaling and Overflow Behavior
          The function is implemented using an internal 32-bit accumulator.
          The accumulator maintains 1.31 format by truncating lower 31 bits
  of the intermediate multiplication in 2.62 format. There is saturation on
  the addition, hence there is no risk of overflow.
*/
#if defined(PLATFORM_GENERIC)
void bear_clarke_q31(q31_t I_a, q31_t I_b, q31_t* I_alpha, q31_t* I_beta);
#else  // RISCV_P_SIMD_EXT
__STATIC_FORCEINLINE void bear_clarke_q31(q31_t I_a, q31_t I_b, q31_t* I_alpha,
                                          q31_t* I_beta)
{
    const q31_t prod1 = __rv_kwmmul_u(I_b, 0x49E69D16);  // q31*(2*q31) >31> q31
    const q31_t prod2 = __rv_kwmmul_u(I_a, 0x24F34E8B);  // q31*(2*q31) >31> q31
    *I_alpha = I_a;
    const q31_t sum = __rv_kaddw(prod1, prod2); // (q31 + q31) =  q31
    // TODO: Use immediate (ksllwi) version
    *I_beta = __rv_ksllw(sum, 1); // SAT(2 * q31)
}
#endif

/**
  @ingroup inv_clarke
  @brief  Floating-point Inverse Clarke transform
  @param[in]  I_alpha  input two-phase orthogonal vector axis alpha
  @param[in]  I_beta   input two-phase orthogonal vector axis beta
  @param[out] I_a     points to output three-phase coordinate <code>a</code>
  @param[out] I_b     points to output three-phase coordinate <code>b</code>
  @return     none
 */
void bear_inv_clarke_f32(float32_t I_alpha, float32_t I_beta, float32_t* I_a,
                         float32_t* I_b);

/**
  @ingroup inv_clarke
  @brief  Inverse Clarke transform for Q31 version
  @param[in]  I_alpha  input two-phase orthogonal vector axis alpha
  @param[in]  I_beta   input two-phase orthogonal vector axis beta
  @param[out] I_a     points to output three-phase coordinate <code>a</code>
  @param[out] I_b     points to output three-phase coordinate <code>b</code>
  @return     none

  \par Scaling and Overflow Behavior
         The function is implemented using an internal 32-bit accumulator.
         The accumulator maintains 1.31 format by truncating lower 31 bits of
  the intermediate multiplication in 2.62 format. There is saturation on the
  subtraction, hence there is no risk of overflow.
 */
#if defined(PLATFORM_GENERIC)
void bear_inv_clarke_q31(q31_t I_alpha, q31_t I_beta, q31_t* I_a, q31_t* I_b);
#else  // RISCV_P_SIMD_EXT
__STATIC_FORCEINLINE void bear_inv_clarke_q31(q31_t I_alpha, q31_t I_beta,
                                              q31_t* I_a, q31_t* I_b)
{
    const q31_t prod1 = __rv_kwmmul_u(I_alpha, 0x40000000);  // q31*q31 >31> q31
    const q31_t prod2 = __rv_kwmmul_u(I_beta, 0x6ED9EBA1);   // q31*q31 >31> q31
    *I_a = I_alpha;
    *I_b = __rv_ksubw(prod2, prod1);  // q31 - q31 = q31
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* ifndef _CONTROLLER_FUNCTIONS_H_ */
