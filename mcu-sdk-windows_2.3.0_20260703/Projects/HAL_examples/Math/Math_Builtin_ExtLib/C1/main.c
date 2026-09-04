/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/Math/Math_Builtin_ExtLib/C1/main.c
 *  @author     Baikal electronics SDK team
 *  @brief      HAL example source file
 *  @version    2.3.0
 *  @date       20260703
 * *****************************************************************************
 *  @copyright Copyright (c) 2025 Baikal Electronics JSC
 *
 *  This software is licensed under terms that can be found in the LICENSE file
 *  in the root directory of this software component.
 *  If no LICENSE file comes with this software, it is provided AS-IS.
 * *****************************************************************************
 */
#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include "bmcu_common.h"

#include "bmcu_rom1_dsp.h"
#include "bear_dsp.h"

#include "bsp.h"

#define FFT_LEN                         256
#define SAMPLE_SIZE                     FFT_LEN
#define SPECTRE_SIZE                    (FFT_LEN / 2)
#define SAMPLE_RATE                     48000.0
#define TONE_FREQ                       13000.0
#define TONE_AMP                        0.25
#define TONE_NONLIN                     0.01

float samples[SAMPLE_SIZE];
float spectre_log[SPECTRE_SIZE];

q15_t samples_q15[SAMPLE_SIZE];
q15_t spectre_fft_q15[SPECTRE_SIZE];
q15_t spectre_abs_q15[SPECTRE_SIZE];
q15_t spectre_log_q15[SPECTRE_SIZE];
q15_t spectre_log_scaled_q15[SPECTRE_SIZE];

int __io_putchar(int ch)
{
    return bsp_serial_putchar(ch);
}

int __io_getchar(void)
{
    return -1;
}

int main(void)
{
    /* Make calculations on Core 0 */
    printf("CORE1:\r\n");

    for (int i = 0; i < SAMPLE_SIZE; i++) {
        float x = sinf(i * 2 * PI * TONE_FREQ / SAMPLE_RATE); /* sinf from built-in on core 0 */
        samples[i] = TONE_AMP * x * (1 - TONE_NONLIN * powf(x, 3)); /* powf is always from external math lib */
        if ((samples[i] >= 1) || (samples[i] < -1))
            printf("%d - !\r\n", i);
    }

    bear_float_to_q15(samples, samples_q15, SAMPLE_SIZE); /* this and others bear_functions are from external DSP-lib function */
    for (int i = 0; i < SAMPLE_SIZE; i++) {
        printf("%d - 0x%x\r\n", i, samples_q15[i]);
    }

    bear_rfft_instance_q15 fft;
    bear_rfft_init_q15(&fft, FFT_LEN, 0, 0);

    printf ("\r\n---\r\n");
    bear_rfft_q15(&fft, samples_q15, spectre_fft_q15);

    bear_cmplx_mag_q15(spectre_fft_q15, spectre_abs_q15, SPECTRE_SIZE);

    /* use dsp_ functions from built-in lib */
    dsp_vlog_q15(spectre_abs_q15, spectre_log_q15, SPECTRE_SIZE);
    dsp_scale_q15(spectre_log_q15, 0x5000 /* 0.625 */, -5, spectre_log_scaled_q15, SPECTRE_SIZE);

    bear_q15_to_float(spectre_log_scaled_q15, spectre_log, SPECTRE_SIZE);

    printf("result spectre is\r\n");
    for(int i = 0; i < SPECTRE_SIZE; i++) {
        printf("%d:\r\n spec_fft_q15 0x%x, spectre_abs_q15 0x%x, spectre_log_q15 0x%x, spectre_log_scaled_q15 0x%x\r\n", i, spectre_fft_q15[i], spectre_abs_q15[i], spectre_log_q15[i], spectre_log_scaled_q15[i]);
    }

    for(;;) {}

	return 0;
}
