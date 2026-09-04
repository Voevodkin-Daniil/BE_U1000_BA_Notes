/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/USB/Device/USB_MSC_Flash/flash_mt25ql128aba.c
 *  @author     Baikal electronics SDK team
 *  @brief      Flash driver source file
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

#include "flash_mt25ql128aba.h"
#include "bmcu_common.h"
#include "qspi.h"

/**
 * @brief Initialize flash.
 */
void FLASH_Init(void)
{
    QSPI1_Init();
}

/**
 * @brief Enter Quad mode.
 */
void FLASH_EnterQuadMode(void)
{
    uint8_t Cmd = FLASH_CMD_ENTER_QUAD_IO_MODE;
    QSPI1_Std_Xfer(&Cmd, 1UL);

    QSPI1_StdToEnh();
}

/**
 * @brief Read Status Register.
 * @returns The Status Register value.
 */
uint8_t FLASH_ReadStatusReg(void)
{
    uint8_t Result;
    QSPI1_Enh_Read(FLASH_CMD_READ_STATUS_REG, 0UL, QSPI_ADDRLEN_0, QSPI_WAITCYCLES_0, &Result, 1UL);

    return Result;
}

/**
 * @brief Read Flag Status Register.
 * @returns The Flag Status Register value.
 */
uint8_t FLASH_ReadFlagStatusReg(void)
{
    uint8_t Result;
    QSPI1_Enh_Read(FLASH_CMD_READ_FLAG_STATUS_REG, 0UL, QSPI_ADDRLEN_0, QSPI_WAITCYCLES_0, &Result, 1UL);

    return Result;
}

/**
 * @brief Clear Flag Status Register.
 */
void FLASH_ClearFlagStatusReg(void)
{
    QSPI1_Enh_Write(FLASH_CMD_CLEAR_FLAG_STATUS_REG, 0UL, QSPI_ADDRLEN_0, NULL, 0UL);
}

/**
 * @brief Set the write enable latch.
 */
void FLASH_WriteEnable(void)
{
    QSPI1_Enh_Write(FLASH_CMD_WRITE_ENABLE, 0UL, QSPI_ADDRLEN_0, NULL, 0UL);
}

/**
 * @brief Clear the write enable latch.
 */
void FLASH_WriteDisable(void)
{
    QSPI1_Enh_Write(FLASH_CMD_WRITE_DISABLE, 0UL, QSPI_ADDRLEN_0, NULL, 0UL);
}

/**
 * @brief Read memory.
 * @param Address The address.
 * @param Buffer The data buffer.
 * @param Length The number of bytes to read.
 */
void FLASH_Read(uint32_t Address, uint8_t* Buffer, uint32_t Length)
{
    QSPI1_Enh_Read(FLASH_CMD_FAST_READ, Address, QSPI_ADDRLEN_24, QSPI_WAITCYCLES_10, Buffer, Length);
}

/**
 * @brief Program page.
 * @param Address The address.
 * @param Buffer The data buffer.
 * @param Length The number of bytes to program (up to page size).
 * @returns 0 if completed successfully, otherwise 1.
 */
uint32_t FLASH_ProgramPage(uint32_t Address, uint8_t* Buffer, uint32_t Length)
{
    uint32_t Status = 0UL;

    if (Length > FLASH_PAGE_SIZE)
        Length = FLASH_PAGE_SIZE;

    FLASH_ClearFlagStatusReg();
    FLASH_WriteEnable();

    QSPI1_Enh_Write(FLASH_CMD_PAGE_PROG, Address, QSPI_ADDRLEN_24, Buffer, Length);

    while ((FLASH_ReadStatusReg() & FLASH_STATUS_REG_WRITE_IN_PROGRESS) != 0x0U);

    if (FLASH_ReadFlagStatusReg() & (FLASH_FLAG_STATUS_REG_PROG | FLASH_FLAG_STATUS_REG_PROTECT))
    {
        FLASH_WriteDisable();
        Status = 1UL;
    }

    return Status;
}

/**
 * @brief Program 4K subsector.
 * @param Address The address.
 * @param Buffer The data buffer.
 * @param Length The number of bytes to program (up to page size).
 * @returns 0 if completed successfully, otherwise 1.
 */
uint32_t FLASH_ProgramSubsector4K(uint32_t Address, uint8_t* Buffer, uint32_t Length)
{
    uint32_t Status = 0UL;
    uint32_t SubsectorAddress = Address & 0xFFFFF000UL;
    uint32_t PageAddress = SubsectorAddress;
    uint32_t Programmed = 0UL;
    uint32_t ToProgram;

    if (Length > FLASH_SUBSECTOR_4K_SIZE)
        Length = FLASH_SUBSECTOR_4K_SIZE;

    while (Length > 0UL)
    {
        ToProgram = ((Length > FLASH_PAGE_SIZE) ? FLASH_PAGE_SIZE : Length);

        Status = FLASH_ProgramPage(PageAddress, &Buffer[Programmed], ToProgram);
        if (Status != 0UL)
            break;

        Programmed += ToProgram;
        Length -= ToProgram;
        PageAddress += FLASH_PAGE_SIZE;
    }

    return Status;
}

/**
 * @brief Erase 4K subsector.
 * @param Address The address.
 * @returns 0 if completed successfully, otherwise 1.
 */
uint32_t FLASH_EraseSubsector4K(uint32_t Address)
{
    uint32_t Status = 0UL;

    FLASH_ClearFlagStatusReg();
    FLASH_WriteEnable();

    /* For correct operation of QSPI1_Enh_Write() the data length        */
    /* must not be 0. As a workaround, the address is sent here as data. */
    uint8_t Buffer[3];
    Buffer[2] = (uint8_t)(Address & 0xFFUL);
    Address >>= 8;
    Buffer[1] = (uint8_t)(Address & 0xFFUL);
    Address >>= 8;
    Buffer[0] = (uint8_t)(Address & 0xFFUL);
    QSPI1_Enh_Write(FLASH_CMD_4KB_SUBSECTOR_ERASE, 0UL, QSPI_ADDRLEN_0, (uint8_t*)&Buffer, 3UL);

    while ((FLASH_ReadStatusReg() & FLASH_STATUS_REG_WRITE_IN_PROGRESS) != 0x0U);

    if (FLASH_ReadFlagStatusReg() & (FLASH_FLAG_STATUS_REG_ERASE | FLASH_FLAG_STATUS_REG_PROTECT))
    {
        FLASH_WriteDisable();
        Status = 1UL;
    }

    return Status;
}
