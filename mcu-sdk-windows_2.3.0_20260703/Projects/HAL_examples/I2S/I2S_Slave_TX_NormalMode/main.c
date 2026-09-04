/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/I2S/I2S_Slave_TX_NormalMode/main.c
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

#include "wav/wav_data.h"

#include "bsp.h"

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
    SPI_InitStruct_TypeDef SPI_InitStruct = {0};
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
   //CRU_SetPinAF(CRU_PORT_B, CRU_PIN_11, CRU_PIN_AF_3);    // I2S1_SDI
   CRU_SetPinAF(CRU_PORT_B, CRU_PIN_12, CRU_PIN_AF_3);    // I2S1_WS
   CRU_SetPinAF(CRU_PORT_B, CRU_PIN_13, CRU_PIN_AF_3);    // I2S1_SCLK

   if (I2S_DeInit(I2S1) != SUCCESS)
   {
    Error_Handler();
   }

   I2S_InitStruct_TypeDef I2S1_InitStruct;

   I2S_StructInit(&I2S1_InitStruct);
   I2S1_InitStruct.DataResolution = I2S_DATA_RESOLUTION_16BIT;

   if (I2S_Init(I2S1, &I2S1_InitStruct) != SUCCESS)
   {
    Error_Handler();
   }
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
    for(int i=0; i<200; i++);

    spi_write_reg(0x4, 0x08);    //set 48kHz 256*fs
    for(int i=0; i<200; i++);

    spi_write_reg(0x6, 0xb0);    //set CHIP_i2s_CTRL set master 16 bits
    for(int i=0; i<200; i++);

    spi_write_reg(0xA, 0x51);    //set i2s out i2s-Dac   i2sin to i2s out
    for(int i=0; i<200; i++);

    spi_write_reg(0xE, 0x00);    //UNmute dac
    for(int i=0; i<200; i++);

    spi_write_reg(0x24, 0x00);   //set wtf&
    for(int i=0; i<200; i++);

    spi_write_reg(0x22, 0x1818); //set hp 0db
    for(int i=0; i<200; i++);

    spi_write_reg(0x2A,0x103);  //set power UP
    for(int i=0;i<200;i++);

    spi_write_reg(0x30,0x6AFF); //set power UP
    for(int i=0;i<200;i++);
}

int main(void)
{
    /* Init serial interface */
    bsp_serial_init();
    
    printf("\r\nI2S example start\r\n");

    /* Init SPI1 for audiocodec*/
    SPI1_Init();

    printf("SPI audiocodec init\r\n");
    spi_audiocodec_init();

    /* Init I2S1 */
    printf("I2S1 init\r\n");
    I2S1_Init();

    /* Start transfer */
    printf("Start wav transfer \r\n");

    for(;;)
    {
        uint32_t wav_data_size = wav_massive[WAV_HEADER_SIZE_U16 - 1];
        wav_data_size <<= 16;
        wav_data_size |= wav_massive[WAV_HEADER_SIZE_U16 - 2];

        uint32_t wav_data_idx = 0;
        /* 1. No DMA mode example */
        /* Fill TX FIFOs by writing data to LTHR and RTHR until filled */
        while (I2S_IsActiveIT_TXFO(I2S1) == 0UL)
        {
            I2S_TransmitDataLeft(I2S1, wav_massive[wav_data_idx++]);
            I2S_TransmitDataRight(I2S1, wav_massive[wav_data_idx++]);
            wav_data_size-=2;
        }

        /* Clear the TX FIFO Data Overrun interrupt */
        I2S_ClearIT_TXFO(I2S1);

        /* When data TX FIFO overrun interrupt is activated
           any data being written is lost while data in the FIFO is preserved.
           It is necessary to restore the indexes of the data being sent */
        wav_data_idx-=2;
        wav_data_size+=2;

        /* 2. Enable transmitter */
        I2S_Enable_TX(I2S1);

        /* 3. Send all data transmitter */
        while(wav_data_size)
        {
            /* 3.1 Wait TX FIFO is empty */
            while (I2S_IsActiveIT_TXFE(I2S1) == 0UL);

            /* 3.2 Add data to TX FIFO */
            I2S_TransmitDataLeft(I2S1, wav_massive[wav_data_idx++]);
            I2S_TransmitDataRight(I2S1, wav_massive[wav_data_idx++]);
            wav_data_size-=2;
        }

        /* 4. Stop I2S TX */
        I2S_Disable_TX(I2S1);

        /* 5. Flush I2S TX FIFO buff*/
        I2S_FlushTransmitFIFO(I2S1);
    }

	return 0;
}
