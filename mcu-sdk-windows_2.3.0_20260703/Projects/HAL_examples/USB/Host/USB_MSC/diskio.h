/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/USB/Host/USB_MSC/diskio.h
 *  @author     Baikal electronics SDK team
 *  @brief      HAL example header file
 *  @version    2.3.0
 *  @date       20260703
 * *****************************************************************************
 *  @copyright Copyright (c) 2026 Baikal Electronics JSC
 *
 *  This software is licensed under terms that can be found in the LICENSE file
 *  in the root directory of this software component.
 *  If no LICENSE file comes with this software, it is provided AS-IS.
 * *****************************************************************************
 */

#ifndef _DISKIO_DEFINED
#define _DISKIO_DEFINED

#include "ff.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Command code for disk_ioctrl function */

/* Generic command (Used by FatFs) */
#define CTRL_SYNC			0	/* Complete pending write process (needed at FF_FS_READONLY == 0) */
#define GET_SECTOR_COUNT	1	/* Get media size (needed at FF_USE_MKFS == 1) */
#define GET_SECTOR_SIZE		2	/* Get sector size (needed at FF_MAX_SS != FF_MIN_SS) */
#define GET_BLOCK_SIZE		3	/* Get erase block size (needed at FF_USE_MKFS == 1) */
#define CTRL_TRIM			4	/* Inform device that the data on the block of sectors is no longer used (needed at FF_USE_TRIM == 1) */

/* Generic command (Not used by FatFs) */
#define CTRL_POWER			5	/* Get/Set power status */
#define CTRL_LOCK			6	/* Lock/Unlock media removal */
#define CTRL_EJECT			7	/* Eject media */
#define CTRL_FORMAT			8	/* Create physical format on the media */

/* MMC/SDC specific ioctl command (Not used by FatFs) */
#define MMC_GET_TYPE		10	/* Get card type */
#define MMC_GET_CSD			11	/* Get CSD */
#define MMC_GET_CID			12	/* Get CID */
#define MMC_GET_OCR			13	/* Get OCR */
#define MMC_GET_SDSTAT		14	/* Get SD status */
#define ISDIO_READ			55	/* Read data form SD iSDIO register */
#define ISDIO_WRITE			56	/* Write data to SD iSDIO register */
#define ISDIO_MRITE			57	/* Masked write data to SD iSDIO register */

/* ATA/CF specific ioctl command (Not used by FatFs) */
#define ATA_GET_REV			20	/* Get F/W revision */
#define ATA_GET_MODEL		21	/* Get model name */
#define ATA_GET_SN			22	/* Get serial number */

/** @brief Disk status. */
typedef enum {
	STA_READY = 0,	/*!< Ready */
	STA_NOINIT,		/*!< Drive not initialized */
	STA_NODISK,		/*!< No medium in the drive */
	STA_PROTECT		/*!< Write protected */
} DSTATUS;

/** @brief Result of disk operation. */
typedef enum {
	RES_OK = 0,		/*!< Success */
	RES_ERROR,		/*!< R/W error */
	RES_WRPRT,		/*!< Write protected */
	RES_NOTRDY,		/*!< Not ready */
	RES_PARERR		/*!< Invalid parameter */
} DRESULT;

/** @brief Initialize a drive. */
DSTATUS disk_initialize(BYTE pdrv);

/** @brief Get drive status. */
DSTATUS disk_status(BYTE pdrv);

/** @brief Read sector(s). */
DRESULT disk_read(BYTE pdrv, BYTE* buff, LBA_t sector, UINT count);

/** @brief Write sector(s). */
DRESULT disk_write(BYTE pdrv, const BYTE* buff, LBA_t sector, UINT count);

/** @brief Perform drive IO control operation. */
DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void* buff);

#ifdef __cplusplus
}
#endif

#endif /* _DISKIO_DEFINED */
