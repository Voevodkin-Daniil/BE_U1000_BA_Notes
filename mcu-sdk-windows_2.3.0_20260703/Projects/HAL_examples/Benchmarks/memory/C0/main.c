/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/Benchmarks/memory/C0/main.c
 *  @author     Baikal electronics SDK team
 *  @brief      Main file for Core 0 memory benchmark
 *  @version    2.3.0
 *  @date       20260703
 * *****************************************************************************
 *  @copyright Copyright (c) 2026 Baikal Electronics
 *
 *  This software is licensed under terms that can be found in the LICENSE file
 *  in the root directory of this software component.
 *  If no LICENSE file comes with this software, it is provided AS-IS.
 * *****************************************************************************
 */

#include "Mem_copy_test_common.h"

#define CORE1_START_ADDRESS             (EFLASH_BASE + 0x00020000UL)


int main(){
	Init();
	*ready_flag = 0;

	Mem_speed_test(0);
	__delay_cycles(500);

    CRU_C1_Start(CORE1_START_ADDRESS);

    /* Waiting while C1 is testing*/
    while (*ready_flag != 1){
    	N_nop_iterations (2000);
    }

	Print_results (0);
	__delay_cycles(500);
	*ready_flag = 2;

	/* Waiting while C1 is printing*/
	while (*ready_flag != 3){
		N_nop_iterations (2000);
	}

	Validate_timer(0);
}
