/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/USB/Device/USB_MSC_TCM/msc_cb.c
 *  @author     Baikal electronics SDK team
 *  @brief      USB MSC class example
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

#include <string.h>
#include "class/msc/msc_device.h"

#define MSC_VENDOR_ID       "TinyUSB"
#define MSC_PRODUCT_ID      "USB Mass Storage"
#define MSC_PRODUCT_REV     "1.0"

#define MSC_STORAGE_ADDRESS     0x40010000UL
#define MSC_STORAGE_SIZE        65536UL
#define MSC_STORAGE_BLOCK_SIZE  512U

/**
 * @brief Callback function that is invoked when INQUIRY command has been received.
 *        It fills in the Vendor ID, Product ID and Product Revision buffers
 *        with the valid values.
 * @param lun The Logical Unit Number (LUN).
 * @param vendor_id The pointer to the Vendor ID buffer.
 * @param product_id The pointer to the Product ID buffer.
 * @param product_rev The pointer to the Product Revision buffer.
 */
void tud_msc_inquiry_cb(uint8_t lun, uint8_t vendor_id[8], uint8_t product_id[16], uint8_t product_rev[4])
{
    (void)lun;

    const char vid[] = MSC_VENDOR_ID;
    const char pid[] = MSC_PRODUCT_ID;
    const char rev[] = MSC_PRODUCT_REV;

    strncpy((char*)vendor_id, vid, strlen(vid));
    strncpy((char*)product_id, pid, strlen(pid));
    strncpy((char*)product_rev, rev, strlen(rev));
}

/**
 * @brief Callback function that is invoked when TEST UNIT READY command has been received.
 * @param lun The Logical Unit Number (LUN).
 * @returns True, if the logical unit is ready, otherwise false.
 */
bool tud_msc_test_unit_ready_cb(uint8_t lun)
{
    (void)lun;

    return true;
}

/**
 * @brief Callback function that is invoked when READ CAPACITY (10) or
 *        READ FORMAT CAPACITIES command has been received.
 *        Upon execution the application updates block count and block size.
 * @param lun The Logical Unit Number (LUN).
 * @param block_count The pointer to the block count.
 * @param block_size The pointer to the block size.
 * @returns None.
 */
void tud_msc_capacity_cb(uint8_t lun, uint32_t *block_count, uint16_t *block_size)
{
    (void)lun;

    *block_count = MSC_STORAGE_SIZE / MSC_STORAGE_BLOCK_SIZE;
    *block_size = MSC_STORAGE_BLOCK_SIZE;
}

/**
 * @brief Callback function that is invoked when START STOP UNIT command has been received.
 * @note  It requests the device server to change the power condition of
 *        the logical unit or load or eject the medium.
 * @param lun The Logical Unit Number (LUN).
 * @param power_condition The power condition.
 * @param start The start bit.
 * @param load_eject The load/eject medium bit.
 * @returns True, if executed successfully, otherwise false.
 */
bool tud_msc_start_stop_cb(uint8_t lun, uint8_t power_condition, bool start, bool load_eject)
{
    (void)lun;
    (void)power_condition;
    (void)start;
    (void)load_eject;

    return true;
}

/**
 * @brief Callback function that is invoked when READ (10) command has been received.
 *        It copies data from the storage to the buffer (up to bufsize).
 * @note  Address = lba * BLOCK_SIZE + offset
 * @note  The offset is only needed if CFG_TUD_MSC_EP_BUFSIZE is smaller
 *        than BLOCK_SIZE.
 * @param lun The Logical Unit Number (LUN).
 * @param lba The logical block address.
 * @param offset The address offset.
 * @param buffer The buffer.
 * @param bufsize The buffer size.
 * @returns The number of bytes read or status:
 *          - 0 < ret < bufsize: These bytes are transferred first and callback
 *                               will be invoked again for remaining data.
 *          - TUD_MSC_RET_BUSY: Not ready. Callback will be invoked again with
 *                              the same parameters later on.
 *          - TUD_MSC_RET_ERROR: Error (such as invalid address). This request
 *                               will be STALLed and SCSI command will fail.
 *          - TUD_MSC_RET_ASYNC: Data I/O will be done asynchronously in background.
 *                               Application should return immediately.
 *                               tud_msc_async_io_done() must be called once I/O
 *                               is done to signal completion.
 */
int32_t tud_msc_read10_cb(uint8_t lun, uint32_t lba, uint32_t offset, void *buffer, uint32_t bufsize)
{
    (void)lun;

    int32_t retval = bufsize;
    uint32_t addr = lba * MSC_STORAGE_BLOCK_SIZE + offset;

    if ((addr + bufsize) <= MSC_STORAGE_SIZE)
    {
        memcpy(buffer, (void*)(MSC_STORAGE_ADDRESS + addr), bufsize);
    }
    else
    {
        retval = TUD_MSC_RET_ERROR;
    }

    return retval;
}

/**
 * @brief Callback function that is invoked when WRITE (10) command has been received.
 *        It copies data from the buffer to the storage (up to bufsize).
 * @note  Address = lba * BLOCK_SIZE + offset
 * @note  The offset is only needed if CFG_TUD_MSC_EP_BUFSIZE is smaller
 *        than BLOCK_SIZE.
 * @param lun The Logical Unit Number (LUN).
 * @param lba The logical block address.
 * @param offset The address offset.
 * @param buffer The buffer.
 * @param bufsize The buffer size.
 * @returns The number of bytes written or status:
 *          - 0 < ret < bufsize: These bytes are transferred first and callback
 *                               will be invoked again for remaining data.
 *          - TUD_MSC_RET_BUSY: Not ready. Callback will be invoked again with
 *                              the same parameters later on.
 *          - TUD_MSC_RET_ERROR: Error (such as invalid address). This request
 *                               will be STALLed and SCSI command will fail.
 *          - TUD_MSC_RET_ASYNC: Data I/O will be done asynchronously in background.
 *                               Application should return immediately.
 *                               tud_msc_async_io_done() must be called once I/O
 *                               is done to signal completion.
 */
int32_t tud_msc_write10_cb(uint8_t lun, uint32_t lba, uint32_t offset, uint8_t *buffer, uint32_t bufsize)
{
    (void)lun;

    int32_t retval = bufsize;
    uint32_t addr = lba * MSC_STORAGE_BLOCK_SIZE + offset;

    if ((addr + bufsize) <= MSC_STORAGE_SIZE)
    {
        memcpy((void*)(MSC_STORAGE_ADDRESS + addr), buffer, bufsize);
    }
    else
    {
        retval = TUD_MSC_RET_ERROR;
    }

    return retval;
}

/**
 * @brief Callback function that is invoked when SCSI command that is
 *        not handled by TinyUSB has been received.
 * @param lun The Logical Unit Number (LUN).
 * @param scsi_cmd The SCSI command contents.
 * @param buffer The buffer for SCSI Data Stage.
 *               - INPUT: application must fill this with response.
 *               - OUTPUT: application holds the data from host.
 * @param bufsize The buffer size.
 * @returns The number of bytes processed (can be zero for no-data command) or
 *          an error. In the case of the error TinyUSB will STALL the corresponding
 *          endpoint and return failed status in command status wrapper phase.
 */
int32_t tud_msc_scsi_cb(uint8_t lun, uint8_t const scsi_cmd[16], void *buffer, uint16_t bufsize)
{
    (void)buffer;
    (void)bufsize;

    int32_t retval = 0;

    switch (scsi_cmd[0])
    {
        case SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL:
        {
            /* Host is about to read/write etc. */
            retval = 0;
        }
        break;

        default:
        {
            /* Set Sense Key to ILLEGAL REQUEST, Additive Sense Code to INVALID COMMAND */
            tud_msc_set_sense(lun, SCSI_SENSE_ILLEGAL_REQUEST, 0x20, 0x00);
            retval = -1;
        }
        break;
    }
    
    return retval;
}
