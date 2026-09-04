/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/I2S/I2S_Slave_RX_DMA_HS_Mode/main.c
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
#include "bmcu_i2s.h"
#include "bmcu_spi.h"
#include "bmcu_dma.h"

#include "bsp.h"

int16_t receive_table[48];

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
}

int __io_putchar(int ch)
{
    return bsp_serial_putchar(ch);
}

int __io_getchar(void)
{
    return -1;
}

void SPI1_Init(void)
{
    /* Enable GPIO1 clock */
    CRU_APB0_EnableClock(CRU_APB0_PERIPH_GPIO0);

    /* Enable SPI1 clock */
    CRU_APB0_EnableClock(CRU_APB0_PERIPH_SPI1);

    /* Configure alternate function of SPI1 pins */
    CRU_SetPinAF(CRU_PORT_A, CRU_PIN_8, CRU_PIN_AF_1);     // MSPI1_CSN
    CRU_SetPinAF(CRU_PORT_A, CRU_PIN_9, CRU_PIN_AF_1);     // MSPI1_SCK
    CRU_SetPinAF(CRU_PORT_A, CRU_PIN_10, CRU_PIN_AF_1);    // MSPI1_MOSI
    CRU_SetPinAF(CRU_PORT_A, CRU_PIN_11, CRU_PIN_AF_1);    // MSPI1_MISO

    /* Disable pull down resistors */
    //CRU_SetPinPull(CRU_PORT_A, CRU_PIN_8, CRU_PIN_PULL_NO);
    CRU->IOPDCR0 &= 0xFFFF0000;

    SPI_DeInit(SPI1);

    //Initialize SPI master for Command WRITE_ENABLE
    SPI_InitStruct_TypeDef SPI_InitStruct;
    SPI_StructInit(&SPI_InitStruct);

    SPI_InitStruct.DataFrameSize = SPI_DATA_FRAME_SIZE_32;
    SPI_InitStruct.XferMode = SPI_XFER_TX;

    SPI_Init(SPI1, &SPI_InitStruct);

    SPI_SetSlaveSelect(SPI1, SPI_SSEL_SELECTED);
}

void I2S1_Init (void)
{
   /* Enable GPIO1 clock */
   CRU_APB1_EnableClock(CRU_APB1_PERIPH_GPIO1);

   /* Enable I2S1 clock */
   CRU_APB1_EnableClock(CRU_APB1_PERIPH_I2S1);

    /* Configure alternate function of I2S1 pins */
   CRU_SetPinAF(CRU_PORT_B, CRU_PIN_10, CRU_PIN_AF_3);    // I2S1_SDO
   CRU_SetPinAF(CRU_PORT_B, CRU_PIN_11, CRU_PIN_AF_3);    // I2S1_SDI
   CRU_SetPinAF(CRU_PORT_B, CRU_PIN_12, CRU_PIN_AF_3);    // I2S1_WS
   CRU_SetPinAF(CRU_PORT_B, CRU_PIN_13, CRU_PIN_AF_3);    // I2S1_SCLK

   if (I2S_DeInit(I2S1) != SUCCESS)
   {
    Error_Handler();
   }

   I2S_InitStruct_TypeDef I2S1_InitStruct;

   I2S_StructInit(&I2S1_InitStruct);
   I2S1_InitStruct.Mode = I2S_MODE_SLAVE_RX;
   I2S1_InitStruct.DataResolution = I2S_DATA_RESOLUTION_16BIT;
   I2S1_InitStruct.DMATransfer = ENABLE;

   if (I2S_Init(I2S1, &I2S1_InitStruct) != SUCCESS)
   {
    Error_Handler();
   }
}

void DMA1_Init (void)
{
    DMA_InitStruct_TypeDef DMA1_InitStruct;
    DMA_StructInit(&DMA1_InitStruct);

    DMA1_InitStruct.Direction = DMA_DIR_PERIPH_TO_MEMORY;
    DMA1_InitStruct.SrcHandshake = DMA_HS_HARDWARE;
    DMA1_InitStruct.SrcAddress = (uint32_t)&(I2S1->RXDMA);
    DMA1_InitStruct.SrcHWHandshakeInterface = 5;
    DMA1_InitStruct.SrcIncMode = DMA_INC_MODE_NO_CHANGE;
    DMA1_InitStruct.SrcBurstLength = DMA_BURST_LENGTH_1;
    DMA1_InitStruct.SrcTransferWidth = DMA_TR_WIDTH_16BITS;

    DMA1_InitStruct.DstAddress = (uint32_t)receive_table;
    DMA1_InitStruct.DstHandshake = DMA_HS_HARDWARE;
    DMA1_InitStruct.DstIncMode = DMA_INC_MODE_INCREMENT;
    DMA1_InitStruct.DstTransferWidth = DMA_TR_WIDTH_16BITS;
    DMA1_InitStruct.DstBurstLength = DMA_BURST_LENGTH_1;
    DMA1_InitStruct.NbData = 48;

    DMA_Init(DMA1, DMA_CH2, &DMA1_InitStruct);

    /* Enable DMA */
    DMA_Enable(DMA1);
}

void spi_write_reg (uint16_t addr,uint16_t data)
{
    SPI_Disable(SPI1);
    SPI_Enable(SPI1);

    SPI_TransmitData32(SPI1, data | (addr<<16));

    while(!SPI_IsActiveFlag(SPI1, SPI_FLAG_TFE));
    while(SPI_IsActiveFlag(SPI1, SPI_FLAG_BUSY));
}

void spi_audiocodec_init (void)
{
    spi_write_reg(0x2, 0x73);    //set power
    for(int i=0; i<100; i++);

    spi_write_reg(0x4, 0x08);    //set 48kHz 256*fs
    for(int i=0; i<100; i++);

    spi_write_reg(0x6, 0xb0);    //set CHIP_i2s_CTRL set master 16 bits
    for(int i=0; i<100; i++);

    spi_write_reg(0xA, 0x10);    //set i2s_dout -> ADC,  dac -> i2s_in, DAP -> ADC, DAP mixer -> ADC
    for(int i=0; i<100; i++);

    spi_write_reg(0xE, 0x00);    //UNmute dac
    for(int i=0; i<100; i++);

    spi_write_reg(0x22, 0x1818); //set hp 0db
    for(int i=0; i<100; i++);

    spi_write_reg(0x24, 0x00);   //Unmute ADC, ADC -> Microphone,
    for(int i=0; i<100; i++);

    spi_write_reg(0x2A,0x103);  //set power UP
    for(int i=0;i<100;i++);

    spi_write_reg(0x30,0x6AFF); //set power UP
    for(int i=0;i<100;i++);
}

int main(void)
{
    /* Init serial interface */
    bsp_serial_init();
    
    printf("\r\nI2S example start...\r\n");

    /* Init SPI1 for audiocodec*/
    SPI1_Init();

    printf("SPI audiocodec init\r\n");
    spi_audiocodec_init();

    /* Init I2S1 */
    printf("I2S1 init\r\n");
    I2S1_Init();

    /* DMA Init */
    printf("DMA1 init\r\n");
    DMA1_Init();

    /* Start receive data */
    printf("Start receive data \r\n");

    for(;;)
    {
        /* 1. Enable DMA handshaking */
        I2S_EnableDMAReq_RX(I2S1);

        /* 2. Enable transmitter */
        I2S_Enable_RX(I2S1);

        /* 3. Enable DMA channel 2*/
        DMA_EnableChannel(DMA1, DMA_CH2);

        /* 4. Waiting for the end of data transfer */
        while (1)
        {
            printf("DMA1 CH2 EN\r\n");
            while(DMA_IsEnabledChannel(DMA1, DMA_CH2));
            printf("DMA1 CH2 DIS\r\n");

            for (uint8_t i = 0; i < (sizeof(receive_table) / 2); i++)
            {
                printf("%u. 0x%04X\r\n", i, receive_table[i]);
            }
            __delay_ms(10UL);

            DMA_SetDataLength(DMA1, DMA_CH2, 48);
            DMA_EnableChannel(DMA1, DMA_CH2);
        }

        /* 5. Stop I2S TX */
        I2S_Disable_TX(I2S1);

        /* 6. Flush I2S TX FIFO buff*/
        I2S_FlushTransmitFIFO(I2S1);
    }

	return 0;
}
