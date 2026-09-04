# Description
DMA example in block transfer mode.  
Send data from SRAM buffer to the SPI insterface.

Execution flow:
* Initialize system clocks:
    * Core0/1 clock: **200 MHz**
    * APBx clocks: **100 MHz**
    * AHB clock: **100 MHz**
* Initialize in-SRAM 32-bit width data buffer `TxBuffer` by consecutive numbers. Buffer contains 255 elements, as this is the maximum allowed block length value.
* If [`USE_IRQ`](#markdown-header-parameters) was defined, configure and enable DMA **interrupts**.
* [Configure SPI](#markdown-header-spi-configuration) interface.
* [Configure DMA](#markdown-header-dma_configuration) channel.
* Main loop execution flow:
    * Refresh DMA **Source address**.
    * Enable preconfigured **DMA channel**.  
    DMA starts to copy data from the source buffer to the SPI Tx FIFO.
    After that SPI transaction starts because of SPI is enabled, Slave Select line is configured (see [Configure SPI](#markdown-header-spi-configuration)) and Tx FIFO is not empty. When **Tx FIFO level** becomes <= **DMA Tx data level** (see [Configure SPI](#markdown-header-spi-configuration)), SPI Tx DMA request occures and data beeing copied from the SRAM buffer `TxBuffer` to the SPI Tx FIFO by the DMA.
    * Wait until SPI transfer complete.
    * Wait `PERIOD` milliseconds.

## DMA configuration
DMA works in block (i.e. no linked list), no FIFO mode.
* Set SRAM located buffer `TxBuffer` as **Source address**.
* Set SPI data register as **Destination address**.
* Set **32-bit** transfer data width.
* Set **burst length** equal to (SPI Tx FIFO depth) - (SPI DMA Tx data level) to avoid SPI Tx FIFO overflow.
* Configure hardware handshake interface according to the used SPI interface (see *Reference manual* for more details).
* Enable DMA itself.

## SPI configuration
* Set **alternate function** mode for CRU pins according to the used SPI master port.
* Set **Tx only** mode.
* Set **32-bit** data width.
* Set the **smallest** permissible clock **divider** to achieve maximum transmission speed.
* Set **DMA Tx data level** (determines Tx FIFO level when DMA request will be sent).
* Enable SPI interface.
* Configure SPI Slave Select line.

When configuration is finished, SPI will transmit data when the Tx FIFO buffer is not empty.

## Parameters
Following parameters can be modified in the project's Makefile:  
`PERIOD`: main loop period (milliseconds).
`USE_IRQ`: if defined, enable DMA interrupts and DMA IRQ handler function. Allows to add DMA status flags processing if required (see `DMA0_IRQHandler` function).

# Hardware configuration
MCU: BE-U1000  
SRAM as RAM memory region  
SPI interface: SPI1 (Master)
|Pin name|SPI signal|
|:---|:---|
|`PA8`|CS *(Chip Select)*|
|`PA9`|SCK *(Clock)*|
|`PA10`|MOSI *(Master Out Slave In)*|
|`PA11`|MISO *(Master In Slave Out)*|

DMA: DMA0, Channel 0
