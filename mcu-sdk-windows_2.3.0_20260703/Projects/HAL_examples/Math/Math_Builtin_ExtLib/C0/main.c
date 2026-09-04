/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/Math/Math_Builtin_ExtLib/C0/main.c
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
#include "bmcu_cru.h"

#include "bmcu_rom1_dsp.h"
#include "bear_dsp.h"

#include "bsp.h"

#define CORE1_START_ADDRESS             (EFLASH_BASE + 0x00020000UL)

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

void CLK_Init(void)
{
    /*
     * Clock configuration:
     *
     * PLL = 200MHz
     * CCLK = 100MHz
     * PCLK0 = 100MHz
     * PCLK1 = 100MHz
     * PCLK2 = 100MHz
     * HCLK = 50MHz
     */

    CRU_PLL_BypassMode_Disable();

    /* Set PLL source clock to C0 */
    CRU_PLL_SetSource(CRU_PLL_SRC_C0);

    /* Configure PLL */
    CRU_PLL_ForceReset();
    CRU_PLL_Config(1UL, 48UL, 6UL, 48UL);
    CRU_PLL_ReleaseReset();

    /* PLL lock time min. 500 cycles */
    __delay_cycles(500UL);

    /* Configure CCLK */
    CRU_SetCCLKPrescaler(CRU_CLK_DIV_2);
    CRU_CCLKPrescaler_Enable();
    CRU_SetCCLKSource(CRU_CLK_SRC_PLL);

    /* Configure PCLK0 */
    CRU_SetPCLK0Prescaler(CRU_CLK_DIV_2);
    CRU_PCLK0Prescaler_Enable();
    CRU_SetPCLK0Source(CRU_CLK_SRC_PLL);

    /* Configure PCLK1 */
    CRU_SetPCLK1Prescaler(CRU_CLK_DIV_2);
    CRU_PCLK1Prescaler_Enable();
    CRU_SetPCLK1Source(CRU_CLK_SRC_PLL);

    /* Configure PCLK2 */
    CRU_SetPCLK2Prescaler(CRU_CLK_DIV_2);
    CRU_PCLK2Prescaler_Enable();
    CRU_SetPCLK2Source(CRU_CLK_SRC_PLL);

    /* Configure HCLK */
    CRU_SetHCLKPrescaler(CRU_CLK_DIV_4);
    CRU_HCLKPrescaler_Enable();
    CRU_SetHCLKSource(CRU_CLK_SRC_PLL);

    /* Configure 1MHz clocks */
    CRU_SetCCLK1MHzPrescaler(99UL);
    CRU_SetHCLK1MHzPrescaler(49UL);
}

void Init(void)
{
    /* Prepare eFlash to 200 MHz CCLK. See PLL configuration */
    EFLASH_Init(200000000);
    
    /* Init clock */
    CLK_Init();

    /* Init serial interface */
    bsp_serial_init();
}

int main(void)
{
    Init();

    /* Make calculations on Core 0 */
    printf("CORE0:\r\n");

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

    /* use bear_ functions due to absence of built-in DSP-lib */
    bear_vlog_q15(spectre_abs_q15, spectre_log_q15, SPECTRE_SIZE);
    bear_scale_q15(spectre_log_q15, 0x5000, -5, spectre_log_scaled_q15, SPECTRE_SIZE);

    bear_q15_to_float(spectre_log_scaled_q15, spectre_log, SPECTRE_SIZE);

    printf("result spectre is\r\n");
    for(int i = 0; i < SPECTRE_SIZE; i++) {
         printf("%d:\r\n spec_fft_q15 0x%x, spectre_abs_q15 0x%x, spectre_log_q15 0x%x, spectre_log_scaled_q15 0x%x\r\n", i, spectre_fft_q15[i], spectre_abs_q15[i], spectre_log_q15[i], spectre_log_scaled_q15[i]);
    }

    /* Start Core 1 */    

    printf ("Starting C1 at 0x%08lX\n", CORE1_START_ADDRESS);
    CRU_C1_Start(CORE1_START_ADDRESS);

    for(;;) {}

	return 0;
}
