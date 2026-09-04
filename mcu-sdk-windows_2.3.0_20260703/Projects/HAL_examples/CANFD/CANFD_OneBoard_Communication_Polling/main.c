/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/CANFD/CANFD_OneBoard_Communication_Polling/main.c
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
#include <string.h>

#include "bmcu_common.h"
#include "bmcu_canfd.h"
#include "bmcu_cru.h"
#include "bmcu_gpio.h"

#include "bsp.h"

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
     * CANx2CLK = 100MHz
     * CANCLK = 50MHz
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

    /* Configure CANx2CLK */
    CRU_SetCANx2CLKPrescaler(CRU_CLK_DIV_2);
    CRU_CANx2CLKPrescaler_Enable();
    CRU_SetCANx2CLKSource(CRU_CLK_SRC_PLL);

    /* Configure 1MHz clocks */
    CRU_SetCCLK1MHzPrescaler(99UL);
    CRU_SetHCLK1MHzPrescaler(49UL);
}

void CANFD0_Init(void)
{
    /* Enable CANFD0 clock */
    CRU_APB0_EnableClock(CRU_APB0_PERIPH_CANFD0);

    /* Enable GPIO0 clock */
    CRU_APB0_EnableClock(CRU_APB0_PERIPH_GPIO0);

    /* Configure alternate function of CANFD0 pins */
    CRU_SetPinAF(CRU_PORT_A, CRU_PIN_14, CRU_PIN_AF_3);
    CRU_SetPinAF(CRU_PORT_A, CRU_PIN_15, CRU_PIN_AF_3);

    /* Deinitialize CANFD instance */
    CANFD_DeInit(CANFD0);
    __delay_cycles(100UL); /* Delay min. 16 APB clock cycles. */

    /* Initialize CANFD instance */
    CANFD_InitStruct_TypeDef CANFD_InitStruct;
    CANFD_StructInit(&CANFD_InitStruct);

    CANFD_InitStruct.Mode = CANFD_OPMODE_NORMAL;
    /* Arbitration Phase Baud Rate = 1Mbps */
    CANFD_InitStruct.AP_Prescaler = 2U;
    CANFD_InitStruct.AP_SJW = 1U;
    CANFD_InitStruct.AP_TS1 = 12U;
    CANFD_InitStruct.AP_TS2 = 12U;
    /* Data Phase Baud Rate = 1Mbps */
    CANFD_InitStruct.DP_Prescaler = 2U;
    CANFD_InitStruct.DP_SJW = 1U;
    CANFD_InitStruct.DP_TS1 = 12U;
    CANFD_InitStruct.DP_TS2 = 12U;
    CANFD_InitStruct.DP_TDC = DISABLE;
    CANFD_InitStruct.DP_TDCOFF = 0U;
    CANFD_InitStruct.BRSD = DISABLE;
    CANFD_InitStruct.DAR = DISABLE;
    CANFD_InitStruct.DPEE = DISABLE;
    CANFD_InitStruct.ABR = ENABLE;

    CANFD_Init(CANFD0, &CANFD_InitStruct);

    /* Initialize acceptance filters */
    CANFD_SetFilterPartition(CANFD0, 0x1F); /* Disable FIFO-1 */

    CANFD_FilterInitStruct_TypeDef CAN_FilterInitStruct;
    CAN_FilterInitStruct.Index = 0U;
    CAN_FilterInitStruct.Ctrl = ENABLE;
    CAN_FilterInitStruct.StdId = 0x123UL;
    CAN_FilterInitStruct.StdIdMask = 0x7FFUL;
    CAN_FilterInitStruct.IDE = CANFD_IDE_STD;
    CAN_FilterInitStruct.CtrlIDE = ENABLE;
    CAN_FilterInitStruct.RTR = CANFD_RTR_DATA;
    CAN_FilterInitStruct.CtrlRTR = DISABLE;
    CAN_FilterInitStruct.SRR = CANFD_RTR_DATA;
    CAN_FilterInitStruct.CtrlSRR = ENABLE;
    CANFD_FilterInit(CANFD0, &CAN_FilterInitStruct);

    /* Enable CANFD instance */
    CANFD_Enable(CANFD0);
}

void CANFD1_Init(void)
{
    /* Enable CANFD1 clock */
    CRU_APB1_EnableClock(CRU_APB1_PERIPH_CANFD1);

    /* Enable GPIO1 clock */
    CRU_APB1_EnableClock(CRU_APB1_PERIPH_GPIO1);

    /* Configure alternate function of CANFD1 pins */
    CRU_SetPinAF(CRU_PORT_B, CRU_PIN_6, CRU_PIN_AF_2);
    CRU_SetPinAF(CRU_PORT_B, CRU_PIN_7, CRU_PIN_AF_2);

    /* Deinitialize CANFD instance */
    CANFD_DeInit(CANFD1);
    __delay_cycles(100UL); /* Delay min. 16 APB clock cycles. */

    /* Initialize CANFD instance */
    CANFD_InitStruct_TypeDef CANFD_InitStruct;
    CANFD_StructInit(&CANFD_InitStruct);

    CANFD_InitStruct.Mode = CANFD_OPMODE_NORMAL;
    /* Arbitration Phase Baud Rate = 1Mbps */
    CANFD_InitStruct.AP_Prescaler = 2U;
    CANFD_InitStruct.AP_SJW = 1U;
    CANFD_InitStruct.AP_TS1 = 12U;
    CANFD_InitStruct.AP_TS2 = 12U;
    /* Data Phase Baud Rate = 1Mbps */
    CANFD_InitStruct.DP_Prescaler = 2U;
    CANFD_InitStruct.DP_SJW = 1U;
    CANFD_InitStruct.DP_TS1 = 12U;
    CANFD_InitStruct.DP_TS2 = 12U;
    CANFD_InitStruct.DP_TDC = DISABLE;
    CANFD_InitStruct.DP_TDCOFF = 0U;
    CANFD_InitStruct.BRSD = DISABLE;
    CANFD_InitStruct.DAR = DISABLE;
    CANFD_InitStruct.DPEE = DISABLE;
    CANFD_InitStruct.ABR = ENABLE;

    CANFD_Init(CANFD1, &CANFD_InitStruct);

    /* Initialize acceptance filters */
    CANFD_SetFilterPartition(CANFD1, 0x1F); /* Disable FIFO-1 */

    CANFD_FilterInitStruct_TypeDef CAN_FilterInitStruct;
    CAN_FilterInitStruct.Index = 0U;
    CAN_FilterInitStruct.Ctrl = ENABLE;
    CAN_FilterInitStruct.StdId = 0x321UL;
    CAN_FilterInitStruct.StdIdMask = 0x7FFUL;
    CAN_FilterInitStruct.IDE = CANFD_IDE_STD;
    CAN_FilterInitStruct.CtrlIDE = ENABLE;
    CAN_FilterInitStruct.RTR = CANFD_RTR_DATA;
    CAN_FilterInitStruct.CtrlRTR = DISABLE;
    CAN_FilterInitStruct.SRR = CANFD_RTR_DATA;
    CAN_FilterInitStruct.CtrlSRR = ENABLE;
    CANFD_FilterInit(CANFD1, &CAN_FilterInitStruct);

    /* Enable CANFD instance */
    CANFD_Enable(CANFD1);
}

void Init(void)
{
    /* Prepare eFlash to 200 MHz CCLK. See PLL configuration */
    EFLASH_Init(200000000);
    
    /* Init clock */
    CLK_Init();

    /* Init LED */
    bsp_led_init();

    /* Init serial interface */
    bsp_serial_init();

    /* Init CANFD */
    CANFD0_Init();
    CANFD1_Init();
}

int main(void)
{
    uint64_t PrevTime0, PrevTime1;
    uint64_t CurrTime;

    CANFD_TxMsg_TypeDef TxMessage;
    uint8_t TxMessageMarker = 0U;

    CANFD_RxMsg_TypeDef RxMessage;

    uint8_t RxFillLevel;

    Init();

    /* Fill in the message to transmit */
    TxMessage.IDE = CANFD_IDE_STD;
    TxMessage.EDL = CANFD_EDL_CAN;
    TxMessage.RTR = CANFD_RTR_DATA;
    TxMessage.BRS = CANFD_BRS_DISABLE;
    TxMessage.DLC = CANFD_DLC_8BYTES;
    TxMessage.EFC = CANFD_EFC_DISABLE;
    TxMessage.MM = TxMessageMarker;

    PrevTime0 = __get_time_u();
    __delay_ms(500UL);
    PrevTime1 = __get_time_u();

    for(;;)
    {
        /* Get current time */
        CurrTime = __get_time_u();

        /* CANFD0 - Transmit a message every 1s */
        if ((CurrTime - PrevTime0) >= 1000000ULL)
        {
            memcpy(TxMessage.Data, &CurrTime, 8);

            TxMessage.StdId = 0x321UL;

            TxMessageMarker++;
            TxMessage.MM = TxMessageMarker;

            uint8_t NbBuf = CANFD_Transmit(CANFD0, &TxMessage);

            /* Debug output*/
            if (NbBuf != CANFD_TX_NO_BUFFER)
            {
                uint8_t Datalen = CANFD_DLCtoLEN(TxMessage.DLC);
                printf("CANFD0 Tx:");
                for (uint8_t i = 0U; i < Datalen; i++)
                    printf(" 0x%02X", TxMessage.Data[i]);
                printf("\r\n");
            }

            PrevTime0 = CurrTime;
        }

        /* CANFD1 - Transmit a message every 1s */
        if ((CurrTime - PrevTime1) >= 1000000ULL)
        {
            memcpy(TxMessage.Data, &CurrTime, 8);

            TxMessageMarker++;
            TxMessage.MM = TxMessageMarker;

            TxMessage.StdId = 0x123UL;

            uint8_t NbBuf = CANFD_Transmit(CANFD1, &TxMessage);

            /* Debug output*/
            if (NbBuf != CANFD_TX_NO_BUFFER)
            {
                uint8_t Datalen = CANFD_DLCtoLEN(TxMessage.DLC);
                printf("CANFD1 Tx:");
                for (uint8_t i = 0U; i < Datalen; i++)
                    printf(" 0x%02X", TxMessage.Data[i]);
                printf("\r\n");
            }

            PrevTime1 = CurrTime;
        }

        /* Poll for the received messages */

        /* CANFD0 */

        /* Get fill level */
        RxFillLevel = CANFD_RX_FIFO_GetFillLevel(CANFD0, CANFD_RX_FIFO0);

        if (RxFillLevel > 0U)
        {
            CANFD_Receive(CANFD0, CANFD_RX_FIFO0, &RxMessage);

            /* Debug output*/
            uint8_t Datalen = CANFD_DLCtoLEN(RxMessage.DLC);
            printf("CANFD0 Rx:");
            for (uint8_t i = 0U; i < Datalen; i++)
                printf(" 0x%02X", RxMessage.Data[i]);
            printf("\r\n");

            bsp_led_toggle();
        }

        /* CANFD1 */

        /* Get fill level */
        RxFillLevel = CANFD_RX_FIFO_GetFillLevel(CANFD1, CANFD_RX_FIFO0);

        if (RxFillLevel > 0U)
        {
            CANFD_Receive(CANFD1, CANFD_RX_FIFO0, &RxMessage);

            /* Debug output*/
            uint8_t Datalen = CANFD_DLCtoLEN(RxMessage.DLC);
            printf("CANFD1 Rx:");
            for (uint8_t i = 0U; i < Datalen; i++)
                printf(" 0x%02X", RxMessage.Data[i]);
            printf("\r\n");
        }
    }

	return 0;
}
