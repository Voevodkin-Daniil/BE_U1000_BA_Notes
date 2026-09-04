#include "../common.h"


#ifndef CORE1_START_ADDRESS
    #define CORE1_START_ADDRESS             ((uint32_t)&_start - (uint32_t)&__HEADER_SIZE__ + (uint32_t)CODE_SHIFT)
#endif


void UART0_Init()
{
    /* Enable UART0 clock */
    CRU_APB0_EnableClock(CRU_APB0_PERIPH_UART0);

    /* Enable GPIO0 clock */
    CRU_APB0_EnableClock(CRU_APB0_PERIPH_GPIO0);

    /* Configure alternate function of UART0 pins */
    CRU_SetPinAF(CRU_PORT_A, CRU_PIN_6, CRU_PIN_AF_1);
    CRU_SetPinAF(CRU_PORT_A, CRU_PIN_7, CRU_PIN_AF_1);

    UART_DeInit(UART0);

    UART_InitStruct_TypeDef UART_InitStruct;
    UART_StructInit(&UART_InitStruct);

    UART_InitStruct.BaudRate = BOUDRATE;
    UART_InitStruct.DataWidth = UART_DATAWIDTH_8B;
    UART_InitStruct.StopBits = UART_STOP_1BIT;
    UART_InitStruct.Parity = UART_PARITY_NONE;
    UART_InitStruct.Transfer9b = UART_TRANSFER_9B_DISABLE;
    UART_InitStruct.CtrlFIFO = DISABLE;

    UART_Init(UART0, &UART_InitStruct);
}

int main (){
	setbuf(stdout, NULL);   ///< Disable output buffering

    /* Prepare eFlash to 200 MHz CCLK. See PLL configuration */
    EFLASH_Init(CLK_CCLK * 1000 * 1000);
#if MEM_REG_ROM != 2
    CLK_Init();
#endif

    UART0_Init();
    mutex_init(uart_mutex);

#if defined(RUN_CORE01_PAR) || defined(RUN_CORE1)
    PRINTF("> Run C1 from %x: %x - %x + %x\r\n", CORE1_START_ADDRESS, &_start, &__HEADER_SIZE__, CODE_SHIFT);

    /* Reset Core 1 */
    CRU_C1_ForceResetAll();

    __delay_ms(100UL);

    /* Start Core 1 */
    CRU_C1_Start(CORE1_START_ADDRESS);
#endif

#ifndef RUN_CORE1
    main_part();
#endif
}