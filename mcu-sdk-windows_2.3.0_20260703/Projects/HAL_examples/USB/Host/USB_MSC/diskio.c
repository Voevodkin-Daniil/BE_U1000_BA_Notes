/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/USB/Host/USB_MSC/diskio.c
 *  @author     Baikal electronics SDK team
 *  @brief      HAL example source file
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

#include "diskio.h"
#include "tusb.h"

static bool disk_busy[CFG_TUH_DEVICE_MAX];

static void wait_for_disk_io(BYTE pdrv);
static bool disk_io_complete_cb(uint8_t dev_addr, tuh_msc_complete_data_t const* cb_data);

static void wait_for_disk_io(BYTE pdrv)
{
    while (disk_busy[pdrv])
    {
        tuh_task();
    }
}

bool disk_io_complete_cb(uint8_t dev_addr, tuh_msc_complete_data_t const* cb_data)
{
    (void)cb_data;

    disk_busy[dev_addr-1U] = false;

    return true;
}

/**
 * @brief Initialize a drive.
 * @param pdrv Physical drive number to identify the drive.
 * @returns The disk status. Can be one of DSTATUS values.
 */
DSTATUS disk_initialize(BYTE pdrv)
{
    disk_busy[pdrv] = false;

    return STA_READY;
}

/**
 * @brief Get drive status.
 * @param pdrv Physical drive number to identify the drive.
 * @returns The disk status. Can be one of DSTATUS values.
 */
DSTATUS disk_status(BYTE pdrv)
{
    return (tuh_msc_mounted(pdrv + 1U) ? STA_READY : STA_NODISK);
}

/**
 * @brief Read sector(s).
 * @param pdrv Physical drive number to identify the drive.
 * @param buff Data buffer to store read data.
 * @param sector Start sector in LBA.
 * @param count Number of sectors to read.
 * @returns The result the operation. Can be one of DRESULT values.
 */
DRESULT disk_read(BYTE pdrv, BYTE* buff, LBA_t sector, UINT count)
{
    (void)buff;
    (void)sector;
    (void)count;

    uint8_t dev_addr = pdrv + 1U;
    uint8_t lun = 0U;

    disk_busy[pdrv] = true;
    tuh_msc_read10(dev_addr, lun, buff, sector, (uint16_t)count, disk_io_complete_cb, 0UL);
    wait_for_disk_io(pdrv);

    return RES_OK;
}

#if FF_FS_READONLY == 0

/**
 * @brief Write sector(s).
 * @param pdrv Physical drive number to identify the drive.
 * @param buff Data to be written.
 * @param sector Start sector in LBA.
 * @param count Number of sectors to write.
 * @returns The result the operation. Can be one of DRESULT values.
 */
DRESULT disk_write(BYTE pdrv, const BYTE* buff, LBA_t sector, UINT count)
{
    (void)pdrv;
    (void)buff;
    (void)sector;
    (void)count;

    uint8_t dev_addr = pdrv + 1U;
    uint8_t lun = 0U;

    disk_busy[pdrv] = true;
    tuh_msc_write10(dev_addr, lun, buff, sector, (uint16_t)count, disk_io_complete_cb, 0UL);
    wait_for_disk_io(pdrv);

    return RES_OK;
}

#endif

/**
 * @brief Perform drive IO control operation.
 * @param pdrv Physical drive number to identify the drive.
 * @param cmd Control code.
 * @param buff Buffer to send/receive control data.
 * @returns The result the operation. Can be one of DRESULT values.
 */
DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void* buff)
{
    DRESULT result = RES_ERROR;

    uint8_t dev_addr = pdrv + 1U;
    uint8_t lun = 0U;

    switch (cmd)
    {
        case GET_SECTOR_COUNT:
        {
            /* Return the number of available sectors */
            *((DWORD*)buff) = (WORD)tuh_msc_get_block_count(dev_addr, lun);
            result = RES_OK;
        }
        break;

        case GET_SECTOR_SIZE:
        {
            /* Return sector size */
            *((WORD*)buff) = (WORD)tuh_msc_get_block_size(dev_addr, lun);
            result = RES_OK;
        }
        break;

        case GET_BLOCK_SIZE:
        {
            /* Return erase block size in unit of sector */
            *((DWORD*)buff) = 1UL;
            result = RES_OK;
        }
        break;

        case CTRL_SYNC:
        case CTRL_TRIM:
        {
            /* Nothing to do for this command. */
            result = RES_OK;
        }
        break;

        default:
        {
            /* Command is not supported */
            result = RES_PARERR;
        }
        break;
    }

    return result;
}
