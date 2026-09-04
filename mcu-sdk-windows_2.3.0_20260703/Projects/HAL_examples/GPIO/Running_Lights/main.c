/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/GPIO/Running_Lights/main.c
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
/*****************************************************************
* Running Lights on EVU-LI exanple                                   *
******************************************************************/
#include <stdint.h>

#include "bmcu_common.h"
#include "bmcu_cru.h"
#include "bmcu_gpio.h"

#include "bsp.h"

#define CYCLE_DELAY_US  1000U // 1 ms

#define LIGHT0_PORT   GPIO0
#define LIGHT1_PORT   GPIO0
#define LIGHT2_PORT   GPIO0
#define LIGHT3_PORT   GPIO2
#define LIGHT0_PIN    GPIO_PIN_13   // EVU-LI: PA13
#define LIGHT1_PIN    GPIO_PIN_12   // EVU-LI: PA12
#define LIGHT2_PIN    GPIO_PIN_5    // EVU-LI: PA5
#define LIGHT3_PIN    GPIO_PIN_9    // EVU-LI: PC9

#define MIN_RATE_MS   2000
#define MAX_RATE_MS   50
#define DFLT_RATE_MS  100

#define MIN_EPOCH_MS   30000
#define MAX_EPOCH_MS   5000
#define DFLT_EPOCH_MS  10000

#define MSG_RATE_MS    1000  // 1 s


void clk_init25(void) {
    /*
     * Clock configuration:
     *
     * PLL = off
     * CCLK = 25MHz
     * PCLK0 = 25MHz
     * PCLK1 = 25MHz
     * PCLK2 = 25MHz
     * HCLK = 25MHz
     */

    CRU_PLL_BypassMode_Disable();

    // /* Set PLL source clock to C0 */
    // CRU_PLL_SetSource(CRU_PLL_SRC_C0);

    // /* Configure PLL */
    CRU_PLL_ForceReset();
    // CRU_PLL_Config(1UL, 48UL, 48UL, 48UL);
    // CRU_PLL_ReleaseReset();
    // /* PLL lock time min. 500 cycles */
    // __delay_cycles(500UL);

    /* Configure CCLK */
    CRU_SetCCLKPrescaler(CRU_CLK_DIV_1);
    CRU_CCLKPrescaler_Enable();
    CRU_SetCCLKSource(CRU_CLK_SRC_C0);
    /* Configure CSR_TIME to 1 us tick at 25 MHz Core clock*/
    CRU_SetCCLK1MHzPrescaler(25);

    /* Configure PCLK0 */
    CRU_SetPCLK0Prescaler(CRU_CLK_DIV_1);
    CRU_PCLK0Prescaler_Enable();
    CRU_SetPCLK0Source(CRU_CLK_SRC_C0);

    /* Configure PCLK1 */
    CRU_SetPCLK1Prescaler(CRU_CLK_DIV_1);
    CRU_PCLK1Prescaler_Enable();
    CRU_SetPCLK1Source(CRU_CLK_SRC_C0);

    /* Configure PCLK2 */
    CRU_SetPCLK2Prescaler(CRU_CLK_DIV_1);
    CRU_PCLK2Prescaler_Enable();
    CRU_SetPCLK2Source(CRU_CLK_SRC_C0);

    /* Configure HCLK */
    CRU_SetHCLKPrescaler(CRU_CLK_DIV_1);
    CRU_HCLKPrescaler_Enable();
    CRU_SetHCLKSource(CRU_CLK_SRC_C0);
}

void putch(int ch){
    bsp_serial_putchar(ch);
}

inline void print_char(const char c) {
    putch(c);
}

void print_str(const char *s) {
    for(int i=0; s[i] && i<1024; i++){putch(s[i]);}
}

void print_int(int x) {
    if(x==0) {putch(' '); putch('0'); return;}
    if(x<0)  {putch('-'); x=(-x);}
    else {putch(' ');}
    int i, y=0;
    for(i=0; x; i++){y=y*10+x%10; x=x/10;}
    for(   ; i; i--){putch('0'+y%10); y=y/10;}
}

void print_hex(uint32_t x) {
  for(int i=0; i<8; i++){
    char d=(x>>28)&0xF;
    putch(((d<10)?'0':('A'-10)) + d);
    x<<=4;
  }
}


void pins_init(GPIO_TypeDef *GPIOx, uint16_t PinMask) {
    volatile uint32_t *port_ptr;
    if(GPIOx==GPIO0) {
        CRU_APB0_EnableClock(CRU_APB0_PERIPH_GPIO0);
        if(PinMask&0x00FF) CRU_SetPinAF(0, PinMask, CRU_PIN_AF_0);
        if(PinMask&0xFF00) CRU_SetPinAF(0, PinMask>>16, CRU_PIN_AF_0);
        port_ptr=&(CRU->IODSCR0);
    }
    else if(GPIOx==GPIO1) {
        CRU_APB1_EnableClock(CRU_APB1_PERIPH_GPIO1);
        if(PinMask&0x00FF) CRU_SetPinAF(1, PinMask, CRU_PIN_AF_0);
        if(PinMask&0xFF00) CRU_SetPinAF(1, PinMask>>16, CRU_PIN_AF_0);
        port_ptr=&(CRU->IODSCR1);
    }
    else if(GPIOx==GPIO2) {
        CRU_APB2_EnableClock(CRU_APB2_PERIPH_GPIO2);
        if(PinMask&0x00FF) CRU_SetPinAF(2, PinMask, CRU_PIN_AF_0);
        if(PinMask&0xFF00) CRU_SetPinAF(2, PinMask>>16, CRU_PIN_AF_0);
        port_ptr=&(CRU->IODSCR2);
    }
    else{
        return;
    }

    // This method does not work for GPIO2
    // GPIO_InitStruct_TypeDef GPIO_InitStruct;
    // GPIO_InitStruct.PinMask = PinMask;
    // GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT;
    // GPIO_Init(GPIO0, &GPIO_InitStruct);
    GPIO_SetPinMode(GPIOx, PinMask, GPIO_MODE_OUTPUT);

    // setting GPIO driver strength
    // // all-pin method
    // WRITE_REG(*port_ptr, 0xFFFFFFFF);
    // pin accurate method - to be checked
    uint32_t ds = READ_REG(*port_ptr);
    uint32_t ds_setting=0x3, pins_left=PinMask;
    while(pins_left) {
        if(pins_left&0x1) ds = (ds & (~ds_setting)) | ds_setting;
        pins_left >>= 1;
        ds_setting <<= 2;
    }
    WRITE_REG(*port_ptr, ds);
}


static uint64_t interval_start_point_us;

void start_interval_us(void){
    interval_start_point_us=__get_time_u();
    // interval_start_point_us=CSR_READ(CSR_TIME);
}

void wait_interval_us(uint64_t us){
    while(__get_time_u()-interval_start_point_us < us);
}


void running(int rate, int count, int dir) {
    static int phase=0;
    if (count%rate) return;
    switch(phase&0x3) {
        case(0):
            GPIO_SetOutputPin(LIGHT0_PORT, LIGHT0_PIN);   GPIO_ResetOutputPin(LIGHT1_PORT, LIGHT1_PIN);
            GPIO_ResetOutputPin(LIGHT2_PORT, LIGHT2_PIN); GPIO_ResetOutputPin(LIGHT3_PORT, LIGHT3_PIN);
            break;
        case(1):
            GPIO_ResetOutputPin(LIGHT0_PORT, LIGHT0_PIN); GPIO_SetOutputPin(LIGHT1_PORT, LIGHT1_PIN);
            GPIO_ResetOutputPin(LIGHT2_PORT, LIGHT2_PIN); GPIO_ResetOutputPin(LIGHT3_PORT, LIGHT3_PIN);
            break;
        case(2):
            GPIO_ResetOutputPin(LIGHT0_PORT, LIGHT0_PIN); GPIO_ResetOutputPin(LIGHT1_PORT, LIGHT1_PIN);
            GPIO_SetOutputPin(LIGHT2_PORT, LIGHT2_PIN);   GPIO_ResetOutputPin(LIGHT3_PORT, LIGHT3_PIN);
            break;
        case(3):
            GPIO_ResetOutputPin(LIGHT0_PORT, LIGHT0_PIN); GPIO_ResetOutputPin(LIGHT1_PORT, LIGHT1_PIN);
            GPIO_ResetOutputPin(LIGHT2_PORT, LIGHT2_PIN); GPIO_SetOutputPin(LIGHT3_PORT, LIGHT3_PIN);
            break;
    }
    if (dir) phase++; else phase--;
}


 int main(int argc, char** argv) {
    (void)argc;
    (void)argv;

    /* Prepare eFlash to 25 MHz CCLK. See PLL configuration */
    EFLASH_Init(25000000);

    clk_init25();
    bsp_serial_init();

    bsp_led_init();
    bsp_led_off();

    pins_init(LIGHT0_PORT, LIGHT0_PIN);
    pins_init(LIGHT1_PORT, LIGHT1_PIN);
    pins_init(LIGHT2_PORT, LIGHT2_PIN);
    pins_init(LIGHT3_PORT, LIGHT3_PIN);
    GPIO_SetOutputPin(LIGHT0_PORT, LIGHT0_PIN);
    GPIO_ResetOutputPin(LIGHT1_PORT, LIGHT1_PIN);
    GPIO_ResetOutputPin(LIGHT2_PORT, LIGHT2_PIN);
    GPIO_SetOutputPin(LIGHT3_PORT, LIGHT3_PIN);

    int rate=DFLT_RATE_MS;
    int epoch=DFLT_EPOCH_MS;
    int count, session;

    print_str("\nLet's go!\n");
    for(count=session=0; 1; count++){
        start_interval_us();

        if(count%MSG_RATE_MS==0) {
            print_str("Running Lights: ");
            print_int(count);
            print_char('\n');
            bsp_led_toggle();
        }

        if(count%epoch==0) {
            session++;
            print_str("Session: ");
            print_int(session);
            print_char('\n');
        }

        switch(session%2){
            case(0): running(rate, count, 0); break;
            case(1): running(rate, count, 1); break;
        }

        //__delay_us(CYCLE_DELAY_US);
        wait_interval_us(CYCLE_DELAY_US);
    }
    return 0;
}
