/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/USB/Device/USB_MSC_Flash/flash_mt25ql128aba.h
 *  @author     Baikal electronics SDK team
 *  @brief      Flash driver header file
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

#ifndef __FLASH_MT25QL128ABA_H
#define __FLASH_MT25QL128ABA_H

/* Flash size */
#define FLASH_SIZE                              (16UL * 1024UL * 1024UL)
#define FLASH_SECTOR_SIZE                       (64UL * 1024UL)
#define FLASH_SUBSECTOR_32K_SIZE                (32UL * 1024UL)
#define FLASH_SUBSECTOR_4K_SIZE                 (4UL * 1024UL)
#define FLASH_PAGE_SIZE                         (256UL)

/* Flash command definitions */

/* Software RESET Operations */
#define FLASH_CMD_RESET_ENABLE                  0x66U
#define FLASH_CMD_RESET_MEMORY                  0x99U
/* READ ID Operations */
#define FLASH_CMD_READ_ID                       0x9EU
#define FLASH_CMD_MULT_IO_READ_ID               0xAFU
#define FLASH_CMD_READ_SFDP                     0x5AU
/* READ MEMORY Operations */
#define FLASH_CMD_READ                          0x03U
#define FLASH_CMD_FAST_READ                     0x0BU
#define FLASH_CMD_DUAL_OUT_FAST_READ            0x3BU
#define FLASH_CMD_DUAL_IN_OUT_FAST_READ         0xBBU
#define FLASH_CMD_QUAD_OUT_FAST_READ            0x6BU
#define FLASH_CMD_QUAD_IN_OUT_FAST_READ         0xEBU
#define FLASH_CMD_DTR_FAST_READ                 0x0DU
#define FLASH_CMD_DTR_DUAL_OUT_FAST_READ        0x3DU
#define FLASH_CMD_DTR_DUAL_IN_OUT_FAST_READ     0xBDU
#define FLASH_CMD_DTR_QUAD_OUT_FAST_READ        0x6DU
#define FLASH_CMD_DTR_QUAD_IN_OUT_FAST_READ     0xEDU
#define FLASH_CMD_QUAD_IN_OUT_WORD_READ         0xE7U
/* WRITE Operations */
#define FLASH_CMD_WRITE_ENABLE                  0x06U
#define FLASH_CMD_WRITE_DISABLE                 0x04U
/* READ REGISTER Operations */
#define FLASH_CMD_READ_STATUS_REG               0x05U
#define FLASH_CMD_READ_FLAG_STATUS_REG          0x70U
#define FLASH_CMD_READ_NVL_CFG_REG              0xB5U
#define FLASH_CMD_READ_VL_CFG_REG               0x85U
#define FLASH_CMD_READ_ENH_VL_CFG_REG           0x65U
#define FLASH_CMD_READ_GPRR                     0x96U
/* WRITE REGISTER Operations */
#define FLASH_CMD_WRITE_STATUS_REG              0x01U
#define FLASH_CMD_WRITE_NVL_CFG_REG             0xB1U
#define FLASH_CMD_WRITE_VL_CFG_REG              0x81U
#define FLASH_CMD_WRITE_ENH_VL_CFG_REG          0x61U
/* CLEAR FLAG STATUS REGISTER Operation */
#define FLASH_CMD_CLEAR_FLAG_STATUS_REG         0x50U
/* PROGRAM Operations */
#define FLASH_CMD_PAGE_PROG                     0x02U
#define FLASH_CMD_DUAL_IN_FAST_PROG             0xA2U
#define FLASH_CMD_EXT_DUAL_IN_FAST_PROG         0xD2U
#define FLASH_CMD_QUAD_IN_FAST_PROG             0x32U
#define FLASH_CMD_EXT_QUAD_IN_FAST_PROG         0x38U
/* ERASE Operations */
#define FLASH_CMD_32KB_SUBSECTOR_ERASE          0x52U
#define FLASH_CMD_4KB_SUBSECTOR_ERASE           0x20U
#define FLASH_CMD_SECTOR_ERASE                  0xD8U
#define FLASH_CMD_BULK_ERASE                    0xC7U
/* SUSPEND/RESUME Operations */
#define FLASH_CMD_SUSPEND                       0x75U
#define FLASH_CMD_RESUME                        0x7AU
/* ONE-TIME PROGRAMMABLE (OTP) Operations */
#define FLASH_CMD_READ_OTP_ARRAY                0x4BU
#define FLASH_CMD_PROG_OTP_ARRAY                0x42U
/* QUAD PROTOCOL Operations */
#define FLASH_CMD_ENTER_QUAD_IO_MODE            0x35U
#define FLASH_CMD_RESET_QUAD_IO_MODE            0xF5U
/* DEEP POWER-DOWN Operations */
#define FLASH_CMD_ENTER_DPD                     0xB9U
#define FLASH_CMD_RELEASE_DPD                   0xABU
/* ADVANCED SECTOR PROTECTION Operations */
#define FLASH_CMD_READ_SECTOR_PROTECTION        0x2DU
#define FLASH_CMD_PROG_SECTOR_PROTECTION        0x2CU
#define FLASH_CMD_READ_VL_LOCK_BITS             0xE8U
#define FLASH_CMD_WRITE_VL_LOCK_BITS            0xE5U
#define FLASH_CMD_READ_NVL_LOCK_BITS            0xE2U
#define FLASH_CMD_WRITE_NVL_LOCK_BITS           0xE3U
#define FLASH_CMD_ERASE_NVL_LOCK_BITS           0xE4U
#define FLASH_CMD_READ_GLOBAL_FREEZE_BIT        0xA7U
#define FLASH_CMD_WRITE_GLOBAL_FREEZE_BIT       0xA6U
#define FLASH_CMD_READ_PASSWORD                 0x27U
#define FLASH_CMD_WRITE_PASSWORD                0x28U
#define FLASH_CMD_UNLOCK_PASSWORD               0x29U
/* ADVANCED FUNCTION INTERFACE Operations */
#define FLASH_CMD_INTERFACE_ACTIVATION          0x9BU
#define FLASH_CMD_CRC                           0x9BU
#define FLASH_CMD_CRC_SUBCMD                    0x27U

/* Status Register */
#define FLASH_STATUS_REG_WRITE_IN_PROGRESS      0x01U
#define FLASH_STATUS_REG_WRITE_EN_LATCH         0x02U
#define FLASH_STATUS_REG_BP                     0x5CU
#define FLASH_STATUS_REG_TOP_BOTTOM             0x20U
#define FLASH_STATUS_REG_WRITE_EN               0x80U

/* Flag Status Register */
#define FLASH_FLAG_STATUS_REG_PROTECT           0x02U
#define FLASH_FLAG_STATUS_REG_PROG_SUSPEND      0x04U
#define FLASH_FLAG_STATUS_REG_PROG              0x10U
#define FLASH_FLAG_STATUS_REG_ERASE             0x20U
#define FLASH_FLAG_STATUS_REG_ERASE_SUSPEND     0x40U
#define FLASH_FLAG_STATUS_REG_PROG_ERASE_CTRL   0x80U

void FLASH_Init(void);
void FLASH_EnterQuadMode(void);
uint8_t FLASH_ReadStatusReg(void);
uint8_t FLASH_ReadFlagStatusReg(void);
void FLASH_ClearFlagStatusReg(void);
void FLASH_WriteEnable(void);
void FLASH_WriteDisable(void);
void FLASH_Read(uint32_t Address, uint8_t* Buffer, uint32_t Length);
uint32_t FLASH_ProgramPage(uint32_t Address, uint8_t* Buffer, uint32_t Length);
uint32_t FLASH_ProgramSubsector4K(uint32_t Address, uint8_t* Buffer, uint32_t Length);
uint32_t FLASH_EraseSubsector4K(uint32_t Address);

#endif /* __FLASH_MT25QL128ABA_H */
