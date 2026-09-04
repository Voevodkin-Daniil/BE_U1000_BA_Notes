/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/QSPI/QSPI_Flash/is25lp032d.h
 *  @author     Baikal electronics SDK team
 *  @brief      ISSI IS25LP032D SPI flash IC definitions file
 *  @version    2.3.0
 *  @date       20260703
 * 
 * *****************************************************************************
 *  @copyright Copyright (c) 2025 Baikal Electronics JSC
 *
 *  This software is licensed under terms that can be found in the LICENSE file
 *  in the root directory of this software component.
 *  If no LICENSE file comes with this software, it is provided AS-IS.
 * *****************************************************************************
 */

#ifndef IS25LP032D_H__
#define IS25LP032D_H__

#define SPI_FLASH_PAGE_SIZE (256U)

enum {
    /* --- WRITE ENABLE Commands --- */
    SPI_FLASH_INS_WREN          = 0x06, /* Write Enable */
    SPI_FLASH_INS_WRDI          = 0x04, /* Write Disable */

    /* --- RESET Commands --- */
    SPI_FLASH_INS_REN           = 0x66, /* Software Reset Enable */
    SPI_FLASH_INS_RMEM          = 0x99, /* Software Reset */

    /* --- IDENTIFICATION Commands --- */
    SPI_FLASH_INS_RDJDID        = 0x9F, /* Read JEDEC ID */
    SPI_FLASH_INS_RDMDID        = 0x90, /* Read Manufacturer & Device ID */
    SPI_FLASH_INS_RDJDIDQ       = 0xAF, /* Read JEDEC ID in QPI mode */
    SPI_FLASH_INS_RDUID         = 0x4B, /* Read Unique ID */
    SPI_FLASH_INS_RDSFDP        = 0x5A, /* Read Serial Flash Discoverable Parameters */

    /* --- DATA READ Commands --- */
    SPI_FLASH_INS_NORD          = 0x03, /* Normal Read Data (Max 50MHz) */
    SPI_FLASH_INS_FRD           = 0x0B, /* Fast Read Data (Max 133MHz) */
    SPI_FLASH_INS_FRDO          = 0x3B, /* Fast Read Dual Output */
    SPI_FLASH_INS_FRDIO         = 0xBB, /* Fast Read Dual I/O */
    SPI_FLASH_INS_FRQO          = 0x6B, /* Fast Read Quad Output */
    SPI_FLASH_INS_FRQIO         = 0xEB, /* Fast Read Quad I/O */

    /* --- DATA READ Commands (DTR - Double Transfer Rate) --- */
    SPI_FLASH_INS_FRDTR         = 0x0D, /* Fast Read DTR */
    SPI_FLASH_INS_FRDDTR        = 0xBD, /* Fast Read Dual I/O DTR */
    SPI_FLASH_INS_FRQDTR        = 0xED, /* Fast Read Quad I/O DTR */

    /* --- PROGRAM DATA Commands --- */
    SPI_FLASH_INS_PP            = 0x02, /* Page Program */
    SPI_FLASH_INS_PPQ           = 0x32, /* Quad Input Page Program (также доступен код 0x38) */

    /* --- ERASE DATA Commands --- */
    SPI_FLASH_INS_SER           = 0x20, /* Sector Erase (4KB) - также доступен код 0xD7 */
    SPI_FLASH_INS_BER32         = 0x52, /* Block Erase (32KB) */
    SPI_FLASH_INS_BER64         = 0xD8, /* Block Erase (64KB) */
    SPI_FLASH_INS_CER    		= 0xC7, /* Bulk Erase */
    SPI_FLASH_INS_CER_ALT       = 0x60, /* Chip Erase - также доступен код 0xC7 */

    /* --- SUSPEND/RESUME Commands --- */
    SPI_FLASH_INS_PERSUS        = 0x75, /* Program/Erase Suspend (также доступен код 0xB0) */
    SPI_FLASH_INS_PERRSM        = 0x7A, /* Program/Erase Resume (также доступен код 0x30) */

    /* --- REGISTER Commands --- */
    SPI_FLASH_INS_RDSR          = 0x05, /* Read Status Register */
    SPI_FLASH_INS_WRSR          = 0x01, /* Write Status Register */
    SPI_FLASH_INS_RDFR          = 0x48, /* Read Function Register */
    SPI_FLASH_INS_WRFR          = 0x42, /* Write Function Register */
    
    /* Управление параметрами чтения (Dummy Cycles, Burst Length) */
    SPI_FLASH_INS_SRPNV         = 0x65, /* Set Read Parameters (Non-Volatile) */
    SPI_FLASH_INS_SRPV          = 0xC0, /* Set Read Parameters (Volatile) - также код 0x63 */
    SPI_FLASH_INS_RDRP          = 0x61, /* Read Read Parameters (Volatile) */
    
    /* Управление расширенными параметрами (Driver Strength) */
    SPI_FLASH_INS_SERPNV        = 0x85, /* Set Extended Read Parameters (Non-Volatile) */
    SPI_FLASH_INS_SERPV         = 0x83, /* Set Extended Read Parameters (Volatile) */
    SPI_FLASH_INS_RDERP         = 0x81, /* Read Extended Read Parameters (Volatile) */
    SPI_FLASH_INS_CLERP         = 0x82, /* Clear Extended Read Register (Clear Error Bits) */

    /* --- SECURITY / INFORMATION ROW Commands --- */
    SPI_FLASH_INS_IRER          = 0x64, /* Erase Information Row */
    SPI_FLASH_INS_IRP           = 0x62, /* Program Information Row */
    SPI_FLASH_INS_IRRD          = 0x68, /* Read Information Row */

    /* --- SECTOR LOCK/UNLOCK Commands --- */
    SPI_FLASH_INS_SECLOCK       = 0x24, /* Sector Lock */
    SPI_FLASH_INS_SECUNLOCK     = 0x26, /* Sector Unlock */

    /* --- PROTOCOL Commands --- */
    SPI_FLASH_INS_QPIEN         = 0x35, /* Enter QPI mode */
    SPI_FLASH_INS_QPIDI         = 0xF5, /* Exit QPI mode */

    /* --- DEEP POWER-DOWN Commands --- */
    SPI_FLASH_INS_DP            = 0xB9, /* Enter Deep Power Down */
    SPI_FLASH_INS_RDPD          = 0xAB, /* Release Deep Power Down / Read Product ID */

    /* --- OTHER --- */
    SPI_FLASH_INS_NOP           = 0x00  /* No Operation */
};

#define REG_STATUS_WRITEINPROGRESS_POS (0UL)
#define REG_STATUS_WRITEINPROGRESS_MSK (1 << REG_STATUS_WRITEINPROGRESS_POS)

/* JEDEC ID (Command 9Fh) definitions for ISSI IS25LP032D */
#define DEVID_LEN                  (3UL)

/* Индексы в массиве ответа */
#define REG_ID_MANUFACTURER_ID_IDX   (0UL)
#define REG_ID_MEMTYPE_IDX           (1UL)
#define REG_ID_MEMCAP_IDX            (2UL)

/* Значение Manufacturer ID (9Dh для ISSI) */
#define REG_ID_MANUFACTURER_ID_VAL            (0x9DU)

/* Типы памяти (Memory Type) */
#define REG_ID_MEMTYPE_3V              (0x60U) /* IS25LP series */
#define REG_ID_MEMTYPE_1V8             (0x70U) /* IS25WP series */

/* Емкость (Capacity ID) - для серии LP/WP рассчитывается как 2^N байт */
#define REG_ID_MEMCAP_256MB               (0x19U) /* 256 Mbit */
#define REG_ID_MEMCAP_128MB               (0x18U) /* 128 Mbit */
#define REG_ID_MEMCAP_64MB                (0x17U) /* 64 Mbit */
#define REG_ID_MEMCAP_32MB                (0x16U) /* 32 Mbit (Твой случай) */
#define REG_ID_MEMCAP_16MB                (0x15U) /* 16 Mbit */

/* Дополнительно: Device ID (Command 90h / ABh) */
#define IS25_DEVICE_ID_LP032D           (0x15U) /* Фиксированный ID для этой модели */

#define REG_STATUS_QE_MSK         (1 << 6)

#define SHSL2 1U        // 30 ns
#define SHSL3 1U        // 40 ns

#endif // IS25LP032D_H__
