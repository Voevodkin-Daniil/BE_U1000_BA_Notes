/*
Copyright 2018 Embedded Microprocessor Benchmark Consortium (EEMBC)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

Original Author: Shay Gal-on
*/
#include "coremark.h"
#include "core_portme.h"

#if VALIDATION_RUN
volatile ee_s32 seed1_volatile = 0x3415;
volatile ee_s32 seed2_volatile = 0x3415;
volatile ee_s32 seed3_volatile = 0x66;
#endif
#if PERFORMANCE_RUN
volatile ee_s32 seed1_volatile = 0x0;
volatile ee_s32 seed2_volatile = 0x0;
volatile ee_s32 seed3_volatile = 0x66;
#endif
#if PROFILE_RUN
volatile ee_s32 seed1_volatile = 0x8;
volatile ee_s32 seed2_volatile = 0x8;
volatile ee_s32 seed3_volatile = 0x8;
#endif
volatile ee_s32 seed4_volatile = ITERATIONS;
volatile ee_s32 seed5_volatile = 0;
/* Porting : Timing functions
        How to capture time and convert to seconds must be ported to whatever is
   supported by the platform. e.g. Read value from on board RTC, read value from
   cpu clock cycles performance counter etc. Sample implementation for standard
   time.h and windows.h definitions included.
*/
CORETIMETYPE
barebones_clock()
{
    return (CORETIMETYPE)get_ticks();
}
/* Define : TIMER_RES_DIVIDER
        Divider to trade off timer resolution and total time that can be
   measured.

        Use lower values to increase resolution, but make sure that overflow
   does not occur. If there are issues with the return value overflowing,
   increase this value.
        */
#define GETMYTIME(_t)              (*_t = barebones_clock())
#define MYTIMEDIFF(fin, ini)       ((fin) - (ini))
#define TIMER_RES_DIVIDER          1
#define SAMPLE_TIME_IMPLEMENTATION 1
#define EE_TICKS_PER_SEC           (CLOCKS_PER_SEC / TIMER_RES_DIVIDER)

/** Define Host specific (POSIX), or target specific global time variables. */
static CORETIMETYPE start_time_val, stop_time_val;

/* Function : start_time
        This function will be called right before starting the timed portion of
   the benchmark.

        Implementation may be capturing a system timer (as implemented in the
   example code) or zeroing some system parameters - e.g. setting the cpu clocks
   cycles to 0.
*/
void
start_time(void)
{
    GETMYTIME(&start_time_val);
}
/* Function : stop_time
        This function will be called right after ending the timed portion of the
   benchmark.

        Implementation may be capturing a system timer (as implemented in the
   example code) or other system parameters - e.g. reading the current value of
   cpu cycles counter.
*/
void
stop_time(void)
{
    GETMYTIME(&stop_time_val);
}
/* Function : get_time
        Return an abstract "ticks" number that signifies time on the system.

        Actual value returned may be cpu cycles, milliseconds or any other
   value, as long as it can be converted to seconds by <time_in_secs>. This
   methodology is taken to accommodate any hardware or simulated platform. The
   sample implementation returns millisecs by default, and the resolution is
   controlled by <TIMER_RES_DIVIDER>
*/
CORE_TICKS
get_time(void)
{
    CORE_TICKS elapsed
        = (CORE_TICKS)(MYTIMEDIFF(stop_time_val, start_time_val));
    return elapsed;
}
/* Function : time_in_secs
        Convert the value returned by get_time to seconds.

        The <secs_ret> type is used to accommodate systems with no support for
   floating point. Default implementation implemented by the EE_TICKS_PER_SEC
   macro above.
*/
secs_ret
time_in_secs(CORE_TICKS ticks)
{
    secs_ret retval = ((secs_ret)ticks) / (secs_ret)EE_TICKS_PER_SEC;
    return retval;
}

ee_u32 default_num_contexts = 1;

/* Function : portable_init
        Target specific initialization code
        Test for some common mistakes.
*/
void
portable_init(core_portable *p, int *argc, char *argv[])
{
/*#error \
    "Call board initialization routines in portable init (if needed), in particular initialize UART!\r\n"
    */

    (void)argc; // prevent unused warning
    (void)argv; // prevent unused warning
    uint32_t ticks;

    if (sizeof(ee_ptr_int) != sizeof(ee_u8 *))
    {
        PRINTF(
            "ERROR! Please define ee_ptr_int to a type that holds a "
            "pointer!\r\n");
    }
    if (sizeof(ee_u32) != 4)
    {
        PRINTF("ERROR! Please define ee_u32 to a 32b unsigned type!\r\n");
    }

    // stopwatch(SW_START);
    ticks = get_ticks();
    PRINTF("CPU start tick counter: %lu\r\r\n", ticks);

    p->portable_id = 1;
}
/* Function : portable_fini
        Target specific final code
*/
void
portable_fini(core_portable *p)
{
    uint32_t i = 0;
    int8_t validation_err = 0;
    uint32_t ticks;

    // Reference values for CoreMark results validation
    // Reference values have were calculated on QEMU emulator
    static struct {
        uint32_t iterations;
        uint16_t crcval;
    } crcfinal[] = {
        {1,     0xe714},
        {2,     0x72be},
        {3,     0x2e87},
        {5,     0xf24c},
        {10,    0xfcaf},
        {30,    0xf8b3},
        {100,   0x988c},
        {1000,  0xd340},
        {10000, 0x988c},
        {100000,0xe9f5},
        {0, 0}
    };

    static uint16_t crcrefval[] = {0xe714, 0x1fd7, 0x8e3a};

    // Get pointer to core_results struct
    core_results *res = (core_results *)(p - offsetof(core_results, port));

    PRINTF("\r\n---------------- CoreMark CRC validation ----------------\r\n");

    // Validate crcfinal value
    i = 0;
    do {
        if (ITERATIONS == crcfinal[i].iterations) {
            if (res->crc != crcfinal[i].crcval) {
                validation_err++;
            };
            break;
        }
    } while (crcfinal[++i].iterations);

    if (!crcfinal[i].iterations) {
        PRINTF("Unsupported ITERATIONS value: %d.\r\nValidation was skipped.\r\nPlease, use one of this values:\r\n ", ITERATIONS);

        i = 0;
        do {
            PRINTF("%ld ", crcfinal[i].iterations);
        } while (crcfinal[++i].iterations);
        PRINTF("\r\n");
        return;
    }

    // Compare core_results members values with reference values
    if (res->crclist != crcrefval[0]) {
        validation_err++;
    }

    if (res->crcmatrix != crcrefval[1]) {
        validation_err++;
    }

    if (res->crcstate != crcrefval[2]) {
        validation_err++;
    }

    PRINTF("CoreMark validation result: %s\r\n", (0 == validation_err) ? "OK" : "FAIL");
    // PRINTF("CoreMark stop, CPU counter: %d\r\r\n", stopwatch(SW_STOP));
    ticks = get_ticks();
    PRINTF("CPU stop tick counter: %lu\r\r\n", ticks);
    p->portable_id = 0;
}


uint32_t
get_ticks (void) {
	uint32_t result;

    __asm__ volatile ("csrr %0, %1"
                                    : "=r"(result)    // Output: register
                                    : "i"(CSR_TICKS)  // Input: constant immediate
                                    :);               // Clobbers: none
    return result;
}


uint32_t
stopwatch (enum stopwatch_e op) {
    static uint32_t timestamp;

    switch (op) {
        case SW_START:
            timestamp = get_ticks();
            break;

        case SW_STOP:
            timestamp = get_ticks() - timestamp;
            break;

        default:
            break;
    }

    return timestamp;
}


/* Function for execution time test purposes
   nop_100_num: required number of iterations, each of 100 NOP instructions

   Example:
        uint32_t ticks;

        ticks = get_ticks();
        nop_iterations();
        ticks = get_ticks() - ticks;
        PRINTF("  ticks: %u\r\r\n", ticks);
 */
inline void
nop_iterations (void) {
    uint32_t ctr = ITERATIONS;

    while (ctr--) {
        __asm__ volatile (
            "nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n "
            "nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n "
            "nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n "
            "nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n "
            "nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n "
            "nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n "
            "nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n "
            "nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n "
            "nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n "
            "nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n "
        );
    };
}
