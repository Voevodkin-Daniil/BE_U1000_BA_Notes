/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/GPIO/GPIO_LEDBlink_C2/main.c
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
#include "bmcu_common.h"
#include "bmcu_cru.h"

/*
Function to be executed by the Core 2

1. C code:

    void
    _start (void) {
        SET_BIT(CORE2_PIO->EN, CORE2_PIO_EN0);  // Set PIO pin 0 as output

        while(1) {
            WRITE_REG(CORE2_PIO->OU, READ_REG(CORE2_PIO->OU) ^ CORE2_PIO_OU0);  // Inverse the pin state
            for (int i = 0; i < 10000000; ++i); // Some delay
        }
    }

2. Assembler code
(built for TCMA of the Core 2: -Os, no build_id, no startup code):

    40007800:	4000a7b7          	lui	a5,0x4000a
    40007804:	4798                	lw	a4,8(a5)
    40007806:	0791                	add	a5,a5,4 # 0x4000a004
    40007808:	00176713          	or	a4,a4,1
    4000780c:	c3d8                	sw	a4,4(a5)
    4000780e:	4798                	lw	a4,8(a5)
    40007810:	00174713          	xor	a4,a4,1
    40007814:	c798                	sw	a4,8(a5)
    40007816:	00989737          	lui	a4,0x989
    4000781a:	68070713          	add	a4,a4,1664 # 0x989680
    4000781e:	177d                	add	a4,a4,-1
    40007820:	ff7d                	bnez	a4,0x4000781e
    40007822:	b7f5                	j	0x4000780e

3. Binary code (little endian):
    b7a70040 9847 9107 13671700 d8c3 9847 13471700 98c7 37979800 13070768 7d17 7dff f5b7

Length: 36 bytes

This precompiled binary code will be copied to the TCMA memory region of the Core 2
*/
uint8_t core2_code[] = {
	0xb7, 0xa7, 0x00, 0x40, 0x98, 0x47, 0x91, 0x07, 
	0x13, 0x67, 0x17, 0x00, 0xd8, 0xc3, 0x98, 0x47, 
	0x13, 0x47, 0x17, 0x00, 0x98, 0xc7, 0x37, 0x97, 
	0x98, 0x00, 0x13, 0x07, 0x07, 0x68, 0x7d, 0x17, 
	0x7d, 0xff, 0xf5, 0xb7, 
};

/** @brief  Function to be executed by the Core 0 */
int
main (void) {
    // Initialize the LED pin as PIO 
    CRU_SetPinAF(CRU_PORT_C, CRU_PIN_0, CRU_PIN_AF_1);
    
    // Reset the Core 2
    CRU_C2_ForceResetAll();

    // Releases Core 2 processor complex reset
    CRU_C2_ReleaseResetCx();
    // Releases Core 2 TCM arbitrator (Front Port) reset
    CRU_C2_ReleaseResetFP();

    // Copy Core 2 binary code to be executed to the Core 2's TCM memory area
    for (size_t i = 0U; i < sizeof(core2_code); ++i) {
        *(__IO uint8_t *)(CORE2_TCMA_SYS_BASE + i) = core2_code[i];
    }
    
    // Start the Core 2 code execution from the TCMA memory region
    CRU_C2_Start(CORE2_TCMA_BASE);

    // infinit loop for the Core 0
    while (1);
	return 0;
}

