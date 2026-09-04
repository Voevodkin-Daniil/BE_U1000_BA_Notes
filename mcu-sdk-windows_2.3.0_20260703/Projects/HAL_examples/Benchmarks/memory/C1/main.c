/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/Benchmarks/memory/C1/main.c
 *  @author     Baikal electronics SDK team
 *  @brief      Main file for Core 1 memory benchmark
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

int main(){
	//Init();

	Mem_speed_test(1);
	__delay_cycles(500);

	*ready_flag = 1;

	/* Waiting while C0 is printing*/
	while (*ready_flag != 2){
		N_nop_iterations (2000);
	}
	Print_results (1);
	__delay_cycles(500);
	*ready_flag = 3;

	Validate_timer(1);
}
