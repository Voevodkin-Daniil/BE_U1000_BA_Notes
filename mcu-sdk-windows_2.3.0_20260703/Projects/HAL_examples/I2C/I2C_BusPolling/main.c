/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/I2C/I2C_BusPolling/main.c
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
#include "bmcu_cru.h"
#include "bmcu_i2c.h"
#include "bmcu_gpio.h"

#include "bsp.h"

#define TIMEOUT                                 1000UL
#define I2C_SLAVE_ADDRESS                       0x55U

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

void I2C1_Init(void)
{
    /* Enable I2C1 clock */
    CRU_APB0_EnableClock(CRU_APB0_PERIPH_I2C1);

    /* Enable GPIO0 clock */
    CRU_APB0_EnableClock(CRU_APB0_PERIPH_GPIO0);

    /* Configure alternate function of I2C1 pins */
    CRU_SetPinAF(CRU_PORT_A, CRU_PIN_14, CRU_PIN_AF_2);  /* I2C1 SCL */
    CRU_SetPinAF(CRU_PORT_A, CRU_PIN_15, CRU_PIN_AF_2);  /* I2C1 SDA */

    /* Configure Pull Up resistors */
    CRU_SetPinPull(CRU_PORT_A, CRU_PIN_14, CRU_PIN_PULL_UP);
    CRU_SetPinPull(CRU_PORT_A, CRU_PIN_15, CRU_PIN_PULL_UP);    

    /* Reset I2C1 data registers */
    while (I2C_DeInit(I2C1) != SUCCESS)
    {
        __delay_ms(100UL);
    }

    /* Init I2C1 */
    I2C_InitStruct_TypeDef I2C_InitStruct;
    I2C_StructInit(&I2C_InitStruct);

    I2C_InitStruct.PeripheralMode = I2C_MODE_MASTER;
    I2C_InitStruct.Speed = I2C_CON_SPEED_FAST;
    I2C_InitStruct.OwnAddrSize = I2C_ADDR_7BITS;
    I2C_InitStruct.TargetAddress = 0x07;
    I2C_InitStruct.SCL_LCNT = 120U;
    I2C_InitStruct.SCL_HCNT = 120U;
    I2C_InitStruct.TxFIFOThreshold = 0U;
    I2C_InitStruct.RxFIFOThreshold = 0U;

    I2C_Init(I2C1, &I2C_InitStruct);

    /* Enable I2C1 */
    I2C_Enable(I2C1);
}

void I2C1_Flush(void)
{
    uint8_t DummyByte;

    while (I2C_IsActiveFlag(I2C1, I2C_FLAG_RFNE) == 1UL)
    {
        DummyByte = I2C_ReceiveData(I2C1);
    }

    (void)DummyByte;
}

ErrorStatus I2C_IsDeviceReady (I2C_TypeDef *I2Cx, uint16_t DevAddress, uint32_t Trials, unsigned long Timeout)
{
    uint32_t i = 0UL;

    // Set address
    MODIFY_REG(I2Cx->TAR, I2C_TAR_TARGET_ADDR, (DevAddress & I2C_TAR_TARGET_ADDR));        
    while ((i < Trials))
    {
        if (I2C_IsActiveFlag(I2Cx, I2C_FLAG_TFNF) == 1UL)
            I2C_TransmitData(I2Cx, 0x0U, I2C_FLAG_CMD_READ);

        while (I2C_IsActiveFlag(I2Cx, I2C_FLAG_TFE) != 1UL);
            __delay_ms(2);  // Wait for TX complete

        if (I2C_IsActiveIT(I2Cx, I2C_IT_TX_ABRT) == 1UL)
        {        
            I2C_ClearIT(I2Cx, I2C_IT_TX_ABRT);
            i++; 
            __delay_ms(Timeout);            
        }       
        else
        {
            if (I2C_IsActiveFlag(I2Cx, I2C_FLAG_RFNE) == 1UL)
            {
                uint16_t temp = 0;
                temp= I2C_ReceiveData(I2Cx);  
                return SUCCESS;              
            }   
            else
            {
                i++;
                __delay_ms(Timeout); 
            }                            
        }
    }
    
    return ERROR;
}

void I2C2_Init(void)
{
    /* Enable I2C2 clock */
    CRU_APB1_EnableClock(CRU_APB1_PERIPH_I2C2);

    /* Enable GPIO1 clock */
    CRU_APB1_EnableClock(CRU_APB1_PERIPH_GPIO1);

    /* Configure alternate function of I2C2 pins */
    CRU_SetPinAF(CRU_PORT_B, CRU_PIN_8, CRU_PIN_AF_2);    /* I2C2 SCL */
    CRU_SetPinAF(CRU_PORT_B, CRU_PIN_9, CRU_PIN_AF_2);    /* I2C2 SDA */

    /* Reset I2C2 data registers */
    while (I2C_DeInit(I2C2) != SUCCESS)
    {
        __delay_ms(100UL);
    }

    /* Init I2C2 */
    I2C_InitStruct_TypeDef I2C_InitStruct;
    I2C_StructInit(&I2C_InitStruct);

    I2C_InitStruct.PeripheralMode = I2C_MODE_SLAVE;
    I2C_InitStruct.Speed = I2C_CON_SPEED_FAST;
    I2C_InitStruct.OwnAddress1 = I2C_SLAVE_ADDRESS;
    I2C_InitStruct.OwnAddrSize = I2C_ADDR_7BITS;
    I2C_InitStruct.SCL_LCNT = 120U;
    I2C_InitStruct.SCL_HCNT = 120U;
    I2C_InitStruct.TxFIFOThreshold = 0U;
    I2C_InitStruct.RxFIFOThreshold = 0U;

    I2C_Init(I2C2, &I2C_InitStruct);

    /* Clear all interrupts */
    I2C_ClearIT_All(I2C2);

    /* Configure I2C interrupts */
    I2C_EnableIT(I2C2, I2C_IT_RD_REQ);
    I2C_EnableIT(I2C2, I2C_IT_RX_FULL);
    I2C_EnableIT(I2C2, I2C_IT_TX_ABRT);
    I2C_DisableIT(I2C2, I2C_IT_RX_UNDER);
    I2C_DisableIT(I2C2, I2C_IT_RX_OVER);
    I2C_DisableIT(I2C2, I2C_IT_TX_OVER);
    I2C_DisableIT(I2C2, I2C_IT_TX_EMPTY);
    I2C_DisableIT(I2C2, I2C_IT_RX_DONE);
    I2C_DisableIT(I2C2, I2C_IT_GEN_CALL);
    I2C_DisableIT(I2C2, I2C_IT_SCL_STUCK_AT_LOW);

    /* Configure and enable I2C interrupt in CLIC */
    CLIC_ConfigIRQ(CLIC_I2C2_IRQn,                 /* Interrupt */
                   CLIC_INTATTR_MODE_MACHINE,       /* Privilege mode */
                   1U,                              /* Level */
                   1U,                              /* Priority */
                   CLIC_INTATTR_SHV_VECTORED,       /* Vector mode */
                   CLIC_INTATTR_TRIG_TYPE_LEVEL,    /* Type */
                   CLIC_INTATTR_TRIG_POL_P);        /* Polarity */

    CLIC_EnableIRQ(CLIC_I2C2_IRQn);

    /* Enable I2C2 */
    I2C_Enable(I2C2);
}

void Init(void)
{
    /* Prepare eFlash to 100 MHz CCLK. See PLL configuration */
    EFLASH_Init(100000000);

    /* Init clock */
    CLK_Init();

    /* Init CLIC */
    CLIC_Config(1U, 1U);
    CLIC_SetLevelThreshold(0U);

    /* Init serial interface */
    bsp_serial_init();

    /* Init I2C master */
    I2C1_Init();

    /* Init I2C slave */
    I2C2_Init();

    /* Enable interrupts globally */
    __enable_irq();
}

int main(void)
{
    ErrorStatus status;

    Init();

    for(;;)
    {
        uint8_t devices_found = 0;
        printf("Scanning I2C bus...\r\n");

        I2C1_Flush();

        /* Poll bus */
        for (uint8_t address = 0x08; address <= 0x77; address++) {                    
            status = I2C_IsDeviceReady (I2C1, address, 3, 10UL);
            
            if (status == SUCCESS) {
                printf("Device found at address: 0x%02X\r\n", address);
                devices_found++;
            } else if (status == ERROR) {
                printf("Timeout at address: 0x%02X\r\n", address);
            }
        }

        printf("Scan complete. Found %d devices.\r\n", devices_found);

        __delay_ms(1000UL);
    }

	return 0;
}

void __attribute__ ((interrupt)) I2C2_IRQHandler(void)
{
    uint8_t ch;

    /* TX_ABRT */
    if (I2C_IsActiveIT(I2C2, I2C_IT_TX_ABRT) == 1UL)
    {
        /* The leftover of data is cleared in Tx FIFO */
        I2C_ClearIT(I2C2, I2C_IT_TX_ABRT);
    }

    /* RD_REQ */
    if (I2C_IsActiveIT(I2C2, I2C_IT_RD_REQ) == 1UL)
    {
        /* Check if there is data to send */
        if ((I2C_IsActiveFlag(I2C2, I2C_FLAG_TFNF) == 1UL))
        {
            /* Write data */
            I2C_TransmitData(I2C2, 0x00, I2C_FLAG_CMD_WRITE);
        }

        I2C_ClearIT(I2C2, I2C_IT_RD_REQ);
    }
}
