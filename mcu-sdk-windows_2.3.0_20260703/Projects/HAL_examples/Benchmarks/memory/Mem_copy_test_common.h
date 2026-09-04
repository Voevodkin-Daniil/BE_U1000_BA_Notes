/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/Benchmarks/memory/Mem_copy_test_common.h
 *  @author     Baikal electronics SDK team
 *  @brief      File with most of testing functions
 *  @version    2.3.0
 *  @date       20260703
 *
 *  Main functions, used in test for both cores
 *
 * *****************************************************************************
 *  @copyright Copyright (c) 2025-2026 Baikal Electronics
 *
 *  This software is licensed under terms that can be found in the LICENSE file
 *  in the root directory of this software component.
 *  If no LICENSE file comes with this software, it is provided AS-IS.
 * *****************************************************************************
 */
 
#include "string.h"
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#ifdef TURN_OFF_PRINTF
#include "printf_off.h"
#endif

#include "bmcu_common.h"
#include "bmcu_cru.h"
#include "bmcu_tim.h"
#include "bmcu_uart.h"
#include "bsp.h"
#include "bmcu_eflash.h"
#include "mt25ql128aba.h"
#include "bmcu_qspi.h"
#include "cli.h"

#define IF_INSTANCE QSPI1

#define REG_READ_RETRY_CNT  (10U)
#define REG_READ_RETRY_INTERVAL  (100U)
/**
 * @brief   Store the current SPI flash state to minimize redundant reg reads
 */
static struct {
    bool std_mode;      ///< Standard or Quad mode
} state = {
    .std_mode = true,
};


#define TCMA_SRC_ADDR 0x4001F000
#define TCMA_DST_ADDR 0x4001E000

#define TCMB_SRC_ADDR 0x4002F000
#define TCMB_DST_ADDR 0x4002E000

#define EFLASH_SRC_ADDR 0xA003F000
#define EFLASH_DST_ADDR 0xA003E000

#define XIP_SRC_ADDR 0x90000000

#define SRAM_SRC_ADDR 0x70001000

#define MICROPY_SRC_ADDR 0xA0810000

#define ROM_LIB_SRC_ADDR 0x4000d604

#define ARR_SIZE 128*3

#define NUM_SRCS 7
#define NUM_DSTS 3

/* ------ */
#define MEM_SET
#define FULL_ARR
#define HALF_OF_ARR
#define QUATER_OF_ARR
#define MY_MEMCPY
#define FOR_HALF
#define FOR_QUATER
#define MEM_READ
#define TIMER_VALIDATION


/* ------ */

int __io_putchar(int ch)
{
    while ((UART_GetLineStatus(BSP_SERIAL_INSTANCE) & UART_LSR_THRE) == 0UL);
    UART_TransmitData8b(BSP_SERIAL_INSTANCE, (uint8_t)ch);

    return ch;
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
    CRU_SetCCLKPrescaler(CRU_CLK_DIV_1);
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

void BSP_UART_Init(void)
{
    /* Enable UART clock */
    CRU_APB0_EnableClock(CRU_APB0_PERIPH_UART0);

    /* Enable GPIO clock */
    CRU_APB0_EnableClock(CRU_APB0_PERIPH_GPIO0);

    /* Configure alternate function of UART pins */
    CRU_SetPinAF(BSP_SERIAL_TX_CRU_PORT, BSP_SERIAL_TX_CRU_PIN, CRU_PIN_AF_1);
    CRU_SetPinAF(BSP_SERIAL_RX_CRU_PORT, BSP_SERIAL_RX_CRU_PIN, CRU_PIN_AF_1);

    UART_DeInit(BSP_SERIAL_INSTANCE);
    
    UART_InitStruct_TypeDef UART_InitStruct;
    UART_StructInit(&UART_InitStruct);

    UART_InitStruct.BaudRate = 115200UL;
    UART_InitStruct.DataWidth = UART_DATAWIDTH_8B;
    UART_InitStruct.StopBits = UART_STOP_1BIT;
    UART_InitStruct.Parity = UART_PARITY_NONE;
    UART_InitStruct.Transfer9b = UART_TRANSFER_9B_DISABLE;
    UART_InitStruct.CtrlFIFO = DISABLE;

    UART_Init(BSP_SERIAL_INSTANCE, &UART_InitStruct);
}

Timer_Init(){
    TIM_InitStruct_TypeDef TIME_IN_STR;
    TIM_StructInit (&TIME_IN_STR);
    TIM_Init(TIM0, TIM_CH0, &TIME_IN_STR);
    TIM_EnableChannel(TIM0, TIM_CH0);
}

void Init(void)
{
	/* Prepare eFlash to 200 MHz CCLK. See PLL configuration */
    EFLASH_Init(200000000);

    /* Init clock */
    CLK_Init();

    /* Init UART */
    #ifndef PRINTF_OFF
    BSP_UART_Init();
    #endif

    /* Init timer */
    Timer_Init();
}

#define PATTERN 0xaaaaaaaa

volatile uint32_t* ready_flag = (uint32_t*) 0x4001DFF0;


void Verify_Writing (uint32_t* writed, uint32_t* original, uint32_t size){
	uint8_t Err = 0;
	for (int k = 0; k < size; ++k)
			if (writed[k] != original[k]){
				Err = 1;
				printf ("ERROR when writing from %p to %p on block %d (0x%x != 0x%x)\n",
				 original, writed, k, original[k], writed[k]);
			}
	printf ("\n%p to %p copied ", original, writed);
	if (! Err){
		printf("OK\n");
	} else {
		printf("WITH ERRORS\n");
	}
	printf ("At %p val is %p, at %p - %p\n", (writed - 1), *(writed - 1), (writed + size), *(writed + size));
}

void Verify_Memset (uint32_t* writed, uint32_t original_val, uint32_t size){
	uint8_t Err = 0;
	for (int k = 0; k < size; ++k)
			if (writed[k] != original_val){
				Err = 1;
				printf ("ERROR when writing to %p on block %d (0x%x != 0x%x)\n",
				 writed, k, original_val, writed[k]);
			}
	printf ("\n%p to %p copied ", original_val, writed);
	if (! Err){
		printf("OK\n");
	} else {
		printf("WITH ERRORS\n");
	}
	printf ("At %p val is %p, at %p - %p\n", (writed - 1), *(writed - 1), (writed + size), *(writed + size));
}

//Testing function to test writing speed
void  __attribute__((noinline)) memset_custom(uint32_t *dest, uint32_t value, int count) {
    __asm__ volatile (
        "mv a0, %0		\n\t"
        "1:                  \n\t"
        "sw %1, 0(a0) \n\t"
        "sw %1, 4(a0) \n\t"
        "sw %1, 8(a0) \n\t"
        "sw %1, 12(a0) \n\t"
        "sw %1, 16(a0) \n\t"
        "sw %1, 20(a0) \n\t"
        "sw %1, 24(a0) \n\t"
        "sw %1, 28(a0) \n\t"
        "sw %1, 32(a0) \n\t"
        "sw %1, 36(a0) \n\t"
        "sw %1, 40(a0) \n\t"
        "sw %1, 44(a0) \n\t"
        "sw %1, 48(a0) \n\t"
        "sw %1, 52(a0) \n\t"
        "sw %1, 56(a0) \n\t"
        "sw %1, 60(a0) \n\t"
        "add %2, %2, -16 \n\t"
        "add a0, a0, 64      \n\t"
        "bnez %2, 1b   \n\t"
        "ret 				\n\t"
        :
        : "r" (dest), "r" (value), "r" (count)
        : "a0"
    );
}


//Testing function to test reading speed
uint32_t  __attribute__((noinline)) memread_custom(uint32_t *src, int count) {
	uint32_t readen;
    __asm__ volatile (
        "mv t0, %1		\n\t"
        "1:                  \n\t"
        "lw %0, 0(t0) \n\t"
        "lw %0, 4(t0) \n\t"
        "lw %0, 8(t0) \n\t"
        "lw %0, 12(t0) \n\t"
        "lw %0, 16(t0) \n\t"
        "lw %0, 20(t0) \n\t"
        "lw %0, 24(t0) \n\t"
        "lw %0, 28(t0) \n\t"
        "lw %0, 32(t0) \n\t"
        "lw %0, 36(t0) \n\t"
        "lw %0, 40(t0) \n\t"
        "lw %0, 44(t0) \n\t"
        "lw %0, 48(t0) \n\t"
        "lw %0, 52(t0) \n\t"
        "lw %0, 56(t0) \n\t"
        "lw %0, 60(t0) \n\t"
        "add %2, %2, -16 \n\t"
        "add t0, t0, 64      \n\t"
        "bnez %2, 1b   \n\t"
        : "=r" (readen)
        : "r" (src), "r" (count)
    );
    return readen;
}

// Function to do nothing for 0.5 us, on 200MHz Core speed
extern inline void N_nop_iterations(uint32_t Iter) {
    while (Iter--) {
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
    }
}

uint32_t Speeds [NUM_SRCS][NUM_DSTS];
uint32_t Ticks [NUM_SRCS][NUM_DSTS];

uint32_t Speeds_memset [NUM_DSTS][3];
uint32_t Ticks_memset [NUM_DSTS][3];

uint32_t Speeds_read [NUM_SRCS];
uint32_t Ticks_read [NUM_SRCS];
char* SRC_Names[] = {"TCMA", "TCMB", "EFLASH", "XIP", "SRAM", "MicroPy", "ROM_LIB"};

uint32_t TIM0_ClkPerTick = 0;
uint32_t TIM1_ClkPerTick = 0;

void Get_PCLK_freq (char TimNum, uint32_t * Clk_perTick){
    CRU_Clocks_TypeDef cru_clocks;
    CRU_GetSystemClocksFreq(&cru_clocks);
    #ifdef PRINT_CLKS
        printf ("CCLK_Frequency = %u\n", cru_clocks.CCLK_Frequency);
        printf ("PCLK0_Frequency = %u\n", cru_clocks.PCLK0_Frequency);
        printf ("PCLK1_Frequency = %u\n", cru_clocks.PCLK1_Frequency);
        printf ("PCLK2_Frequency = %u\n", cru_clocks.PCLK2_Frequency);
        printf ("HCLK_Frequency = %u\n", cru_clocks.HCLK_Frequency);
        printf ("TCLK_Frequency = %u\n", cru_clocks.TCLK_Frequency);
        printf ("CANx2CLK_Frequency = %u\n", cru_clocks.CANx2CLK_Frequency);
    #endif
    if (TimNum == 0){
        *Clk_perTick = cru_clocks.PCLK0_Frequency; /* TIM0 is being clocked by PCLK0 */
    }
    else if(TimNum == 1){
        *Clk_perTick = cru_clocks.PCLK1_Frequency; /* TIM1 is being clocked by PCLK1 */
    }
}

uint32_t tick_to_time_recount(TIM_TypeDef* TIMx, uint32_t ticks, uint32_t period){
    if (TIM0 == TIMx) /* TIM0 is being clocked by PCLK0 */
    {
        if (! TIM0_ClkPerTick)
            Get_PCLK_freq(0, &TIM0_ClkPerTick);
        return ticks / (TIM0_ClkPerTick / period);
    }
    else if (TIM1 == TIMx) /* TIM1 is being clocked by PCLK1 */
    {
        if (! TIM1_ClkPerTick)
            Get_PCLK_freq(1, &TIM1_ClkPerTick);
        return ticks / (TIM1_ClkPerTick / period);
    }
    else {
        return -1 ; /* If TIMx is unknown */
    }
}


int Mem_speed_test(int CoreNum){
    printf ("CORE%d:\nStarting tests\n", CoreNum);


    //bsp_led_init();

    for (int z = 0; z < 2; ++z){
            //bsp_led_toggle();

        __delay_ms(500UL);
        for (int i = 0; i < NUM_SRCS; ++i)
        {

            uint32_t SRC_Addr, DST_Addr;
            switch (/*(i + z + NUM_SRCS - 1) % NUM_SRCS*/ i){
            case 0:
                SRC_Addr = TCMA_SRC_ADDR;
                printf ("\n=====\nCopying  TCMA ----> \n");
                break;
            case 1:
                SRC_Addr = TCMB_SRC_ADDR;
                printf ("\n=====\nCopying  TCMB ----> \n");
                break;
            case 2:
                SRC_Addr = EFLASH_SRC_ADDR;
                printf ("\n=====\nCopying  EFLASH ----> \n");
                break;
            case 3:
                qspi_if_init (NULL);
                flash_set_xip_cmd ("on");
                SRC_Addr = XIP_SRC_ADDR;
                printf ("\n=====\nCopying  XIP ----> \n");
                break;
            case 4:
                SRC_Addr = SRAM_SRC_ADDR;
                printf ("\n=====\nCopying  SRAM ----> \n");
                break;
            case 5:
                SRC_Addr = MICROPY_SRC_ADDR;
                printf ("\n=====\nCopying  MicroPy ----> \n");
                break;
            case 6:
                SRC_Addr = ROM_LIB_SRC_ADDR;
                printf ("\n=====\nCopying  ROM_LIB ----> \n");
                break;
            }
            uint32_t* SRC_Arr = (uint32_t*)SRC_Addr;
            if (i < 3){
                for (int k = 0; k < ARR_SIZE; ++k)
                    *(SRC_Arr + k) = PATTERN;   //test_data[k];
            }

            for (int j = 0; j < NUM_DSTS; ++j){
                
                switch (j) {
                case 0:
                    DST_Addr = TCMA_DST_ADDR;
                    printf ("-> TCMA\n");
                    break;
                case 1:
                    DST_Addr = TCMB_DST_ADDR;
                    printf ("-> TCMB\n");
                    break;
                case 2:
                    DST_Addr = EFLASH_DST_ADDR;
                    EFLASH_EraseBlock(EFLASH_DST_ADDR, EFLASH_MAIN_ARRAY);
                    printf ("-> EFLASH\n");
                    break;
                }

                uint32_t* DST_Arr = (uint32_t*)DST_Addr;


                uint32_t start_time, end_time, op_time, op_time_ms;


        #ifdef MEM_SET
                    if (0 == i){
                        for (int k = 0; k < 3; ++k){
                            if (DST_Addr == EFLASH_DST_ADDR)
                                EFLASH_EraseBlock(EFLASH_DST_ADDR, EFLASH_MAIN_ARRAY);

                            const int cur_arr_size = (ARR_SIZE << k);
                            printf("testing memset\n");
                            start_time = TIM_GetCounter(TIM0, TIM_CH0);
                            memset_custom (DST_Arr, PATTERN, cur_arr_size);
                            end_time = TIM_GetCounter(TIM0, TIM_CH0);

                            op_time = start_time - end_time;
                            op_time_ms = tick_to_time_recount(TIM0, op_time, 1000*1000);
                            Verify_Memset(DST_Arr, PATTERN, cur_arr_size);
                            printf ("Memset time %u ticks (%u us)\n--\n", op_time, op_time_ms);

                            Ticks_memset[j][k] = op_time;
                            Speeds_memset[j][k] = (ARR_SIZE * 4 * 1000000) / op_time * 100 << k;
                        }
                    }
        #endif


        #ifdef FULL_ARR
                    if (DST_Addr == EFLASH_DST_ADDR)
                        EFLASH_EraseBlock(EFLASH_DST_ADDR, EFLASH_MAIN_ARRAY);
                    //Starting time counter
                    start_time = TIM_GetCounter(TIM0, TIM_CH0);
                    memcpy(DST_Arr, SRC_Arr, ARR_SIZE * 4);
                    end_time = TIM_GetCounter(TIM0, TIM_CH0);
                    //Ending time counter

                    op_time = start_time - end_time; //timer is downcounting

                    op_time_ms = tick_to_time_recount(TIM0, op_time, 1000*1000);

                    Verify_Writing(DST_Arr, SRC_Arr, ARR_SIZE);
                    printf ("STD_memcpy time %u ticks (%u us)\n--\n", op_time, op_time_ms);
                    //if (z - 1 ==  i){
                        Ticks[i][j] = op_time;
                        Speeds[i][j] = (ARR_SIZE * 4 * 1000000) / op_time * 100;
                    //}

        #endif

        #ifdef HALF_OF_ARR
                    if (DST_Addr == EFLASH_DST_ADDR)
                        EFLASH_EraseBlock(EFLASH_DST_ADDR, EFLASH_MAIN_ARRAY);
                    //Starting time counter
                    start_time = TIM_GetCounter(TIM0, TIM_CH0);
                    memcpy(DST_Arr, SRC_Arr, ARR_SIZE * 2);
                    end_time = TIM_GetCounter(TIM0, TIM_CH0);
                    //Ending time counter

                    op_time = start_time - end_time;
                    op_time_ms = tick_to_time_recount(TIM0, op_time, 1000*1000);

                    Verify_Writing(DST_Arr, SRC_Arr, ARR_SIZE / 2);
                    printf ("1/2 of memcpy time %u ticks (%u us)\n--\n", op_time, op_time_ms);
        #endif

        #ifdef QUATER_OF_ARR
                    if (DST_Addr == EFLASH_DST_ADDR)
                        EFLASH_EraseBlock(EFLASH_DST_ADDR, EFLASH_MAIN_ARRAY);
                    //Starting time counter
                    start_time = TIM_GetCounter(TIM0, TIM_CH0);
                    memcpy(DST_Arr, SRC_Arr, ARR_SIZE);
                    end_time = TIM_GetCounter(TIM0, TIM_CH0);
                    //Ending time counter

                    op_time = start_time - end_time;
                    op_time_ms = tick_to_time_recount(TIM0, op_time, 1000*1000);

                    Verify_Writing(DST_Arr, SRC_Arr, ARR_SIZE / 4);
                    printf ("1/4 of memcpy time %u ticks (%u us)\n--\n", op_time, op_time_ms);
        #endif

        #ifdef MY_MEMCPY
                    if (DST_Addr == EFLASH_DST_ADDR)
                        EFLASH_EraseBlock(EFLASH_DST_ADDR, EFLASH_MAIN_ARRAY);
                    //Starting time counter
                    start_time = TIM_GetCounter(TIM0, TIM_CH0);
                    for (int b = 0; b < ARR_SIZE; ++b){
                        DST_Arr[b] = SRC_Arr[b];
                    }

                    end_time = TIM_GetCounter(TIM0, TIM_CH0);
                    //Ending time counter

                    op_time = start_time - end_time;
                    op_time_ms = tick_to_time_recount(TIM0, op_time, 1000*1000);

                    Verify_Writing(DST_Arr, SRC_Arr, ARR_SIZE);
                    printf ("Copying by \"for\" cycle time %u ticks (%u us)\n--\n", op_time, op_time_ms);
        #endif

        #ifdef FOR_HALF
                    if (DST_Addr == EFLASH_DST_ADDR)
                        EFLASH_EraseBlock(EFLASH_DST_ADDR, EFLASH_MAIN_ARRAY);
                    //Starting time counter
                    start_time = TIM_GetCounter(TIM0, TIM_CH0);
                    for (int b = 0; b < ARR_SIZE / 2; ++b){
                        DST_Arr[b] = SRC_Arr[b];
                    }

                    end_time = TIM_GetCounter(TIM0, TIM_CH0);
                    //Ending time counter

                    op_time = start_time - end_time;
                    op_time_ms = tick_to_time_recount(TIM0, op_time, 1000*1000);

                    Verify_Writing(DST_Arr, SRC_Arr, ARR_SIZE / 2);
                    printf ("1/2 of copying by \"for\" cycle time %u ticks (%u us)\n--\n", op_time, op_time_ms);
        #endif

        #ifdef FOR_QUATER
                    if (DST_Addr == EFLASH_DST_ADDR)
                        EFLASH_EraseBlock(EFLASH_DST_ADDR, EFLASH_MAIN_ARRAY);
                    //Starting time counter
                    start_time = TIM_GetCounter(TIM0, TIM_CH0);
                    for (int b = 0; b < ARR_SIZE / 4; ++b){
                        DST_Arr[b] = SRC_Arr[b];
                    }

                    end_time = TIM_GetCounter(TIM0, TIM_CH0);
                    //Ending time counter

                    op_time = start_time - end_time;
                    op_time_ms = tick_to_time_recount(TIM0, op_time, 1000*1000);

                    Verify_Writing(DST_Arr, SRC_Arr, ARR_SIZE / 4);
                    printf ("1/4 of copying by \"for\" cycle time %u ticks (%u us)\n--\n", op_time, op_time_ms);
        #endif
    //          }


        #ifdef MEM_READ
                    if (j == 2){
                        volatile uint32_t tmp_val;
                        start_time = TIM_GetCounter(TIM0, TIM_CH0);
                        tmp_val = memread_custom(SRC_Arr, ARR_SIZE);
                        end_time = TIM_GetCounter(TIM0, TIM_CH0);

                        printf ("readen value is %u\n", tmp_val);

                        op_time = start_time - end_time;
                        op_time_ms = tick_to_time_recount(TIM0, op_time, 1000*1000);
                        printf ("MemRead time %u ticks (%u us)\n--\n", op_time, op_time_ms);

                        //if (1){
                            Ticks_read[i] = op_time;
                            Speeds_read[i] = (ARR_SIZE * 4 * 1000000) / op_time * 100;
                        //}
                    }
        #endif
                printf ("-----\n");
                


            }
        }
    }

    return 0;
}

int Print_results(int CoreNum){
    printf ("CORE%d:\n", CoreNum);
    printf("memcpy speed table, B/s (Tiks, 10^-8 s):\n");
    printf("from\\to\t||\tTCMA\t\t||\tTCMB\t\t||\tEFLASH\t|\n");
    for (int i = 0; i < NUM_SRCS; ++i)
    {
        printf("%s\t|", SRC_Names[i]);
        for (int j = 0; j < NUM_DSTS; ++j){
            printf ("|\t%u (%u)\t|", Speeds[i][j], Ticks[i][j]);
        }
        printf ("\n");
    }

#ifdef MEM_SET
    printf("\n\nMemset speed table, B/s (Tiks, 10^-8 s):\n");
    printf("size\\to\t||\tTCMA\t\t||\tTCMB\t\t||\tEFLASH\t|\n");
    for (int k = 0; k < 3; ++k){
        printf ("%d B \t|", (ARR_SIZE * 4 << k));
        for (int j = 0; j < NUM_DSTS; ++j){
            printf ("|\t%u (%u)\t|", Speeds_memset[j][k], Ticks_memset[j][k]);
        }
        printf ("\n");
    }
#endif

#ifdef MEM_READ
    printf("\n\nRead Mem speed table, B/s (Tiks, 10^-8 s):\n");
    for (int i = 0; i < NUM_SRCS; ++i)
    {
        printf("%s\t|", SRC_Names[i]);
        printf ("|\t%u (%u)\t|", Speeds_read[i], Ticks_read[i]);
        printf ("\n");
    }
#endif

    return 0;
}

int Validate_timer(int CoreNum){
    #ifdef TIMER_VALIDATION
        printf ("\n\nCORE%d:\n", CoreNum);
        printf ("Timer validation:\n");
        uint32_t clk_per_tick;
        Get_PCLK_freq(0, &clk_per_tick);
        printf ("%u\n", clk_per_tick);
        uint32_t s10 = 0;
        while (s10 < 420){
            N_nop_iterations(2000000);
            s10 = TIM_GetCounter(TIM0, TIM_CH0) / 10000000;
            printf("%u.%u s\n", s10 / 10, s10 % 10);
            //bsp_led_toggle();
        }
    #endif

    return 0;
}
