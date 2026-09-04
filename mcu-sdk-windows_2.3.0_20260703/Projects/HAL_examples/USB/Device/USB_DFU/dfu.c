/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/USB/Device/USB_DFU/dfu.c
 *  @author     Baikal electronics SDK team
 *  @brief      USB DFU class FSM implementation
 *  @version    2.3.0
 *  @date       20260703
 * 
 *  File content:
 *      - constants definitions
 *      - DFU FSM state description struct
 *      - DFU init/deinit functions
 *          dfu_init()
 *          dfu_deinit()
 *      - DFU FSM functions 
 *          dfu_exec()
 *          tud_dfu_get_timeout_cb()
 *          tud_dfu_download_cb()
 *          tud_dfu_upload_cb()
 *          tud_dfu_manifest_cb()
 *          tud_dfu_abort_cb()
 *          tud_dfu_detach_cb()
 * *****************************************************************************
 *  @copyright Copyright (c) 2025 Baikal Electronics JSC
 *
 *  This software is licensed under terms that can be found in the LICENSE file
 *  in the root directory of this software component.
 *  If no LICENSE file comes with this software, it is provided AS-IS.
 * *****************************************************************************
 */

#include "tusb.h"
#include "dfu.h"
#include "bmcu_cru.h"
#include "bmcu_eflash.h"
#include "dfu.h"
#include "qspi.h"

// ----- Local definnitions, enumerations and variables ------------------------
#define DFU_TIMEOUT_TCM       0
#define DFU_TIMEOUT_EFLASH    0
#define DFU_TIMEOUT_QSPIFLASH 0

/// offset to save firmware's RAM data in case if TCMB used as RAM
#define BASE_ADDR_TCM       0x4002C000UL

#define BASE_ADDR_EFLASH    0xA0000000UL
/// QSPI1 XIP used
#define BASE_ADDR_QSPIFLASH 0x90000000UL

#define MEM_SIZE_TCM       (1024 * 32)
#define MEM_SIZE_EFLASH    (1024 * 256)
#define MEM_SIZE_QSPIFLASH (1024 * 1024 * 16)

/**
    @brief  DFU post-delay
    @note   DFU data transfer have been completed, dfu-util checks DFU device
            status again.
            It expects DFU_IDLE state of DFU device.
            But there is no API to get DFU state at the current tinyUSB revisioin.
            So after DFU manifestation stage the main tud loop will be executed
            DFUUTIL_POSTPROC_CNT times before return */
#define USB_POSTPROC_CNT 1000000

/// DFU FSM states
typedef enum {
    STAGE_IDLE = 0,
    STAGE_INIT = 1,
    STAGE_EXEC = 2,
    STAGE_DONE = 3
} stage_e;

/// Defined by "alt" parameter in DFU request
typedef enum {
    DST_TCM       = 0,
    DST_EFLASH    = 1,
    DST_QSPIFLASH = 2,
    DST_NUM
} mem_dst_e;

/// Set default DFU state
static struct dfu_struct {
    stage_e   dfu_stage;
    mem_dst_e mem_dst;
    bool      eflash_erased;
    bool      qspiflash_erased;
    bool      irq_origstate;
    uint32_t  upload_cnt[DST_NUM];
    uint32_t  dnload_cnt[DST_NUM];
} dfu_state_s = {
    .dfu_stage        = STAGE_IDLE,
    .mem_dst          = DST_TCM,
    .eflash_erased    = false,
    .qspiflash_erased = false,
    .irq_origstate    = false
};

// Device callbacks ------------------------------------------------------------
/**
 *  @brief  Invoked when device is mounted
 */
void
tud_mount_cb (void) {
    logs(VERBOSITY_INFO, "USB mount callback\r\n");
}

/**
 *  @brief  Invoked when device is unmounted
 */
void
tud_umount_cb (void) {
    logs(VERBOSITY_INFO, "USB unmount callback\r\n");
}

// DFU related functions -------------------------------------------------------
int
dfu_init (void) {
    memset (&dfu_state_s, 0, sizeof (dfu_state_s));
    dfu_state_s.dfu_stage     = STAGE_IDLE;
    dfu_state_s.mem_dst       = DST_TCM;
    dfu_state_s.irq_origstate = (CSR_MSTATUS_MIE_Msk & CSR_READ(CSR_MSTATUS));

    logs(VERBOSITY_INFO, "DFU init. IRQ state: %s\r\n",
                  (true == dfu_state_s.irq_origstate) ? "enabled" : "disabled");

    logs(VERBOSITY_INFO, "USB PHY enable and reset release\r\n");
    CRU_USB_PHY_PLL_Enable();
    __delay_cycles(100UL);
    CRU_USB_PHY_ReleaseReset();
    __delay_us(500UL);
    
    CLIC_Config(1U, 1U);
    CLIC_SetLevelThreshold(0U);
    __enable_irq();

    logs(VERBOSITY_INFO, "tinyUSB device init: ");
    tusb_rhport_init_t dev_init = {
        .role = TUSB_ROLE_DEVICE,
        .speed = TUSB_SPEED_AUTO
    };
    tusb_init(BOARD_TUD_RHPORT, &dev_init);
    logs(VERBOSITY_INFO, "done\r\n");

    return 0;
}

int
dfu_deinit (void) {
    logs(VERBOSITY_INFO, "tinyUSB device deinit: ");
    tud_deinit(BOARD_TUD_RHPORT);
    logs(VERBOSITY_INFO, "done\r\n");

    logs(VERBOSITY_INFO, "USB PHY disable, force reset signal\r\n");
    CRU_USB_PHY_ForceReset();
    CRU_USB_PHY_ReleaseReset();

    if (false == dfu_state_s.irq_origstate) {
        logs(VERBOSITY_INFO, "Disable global IRQ\r\n");
        __disable_irq();
    }

    return 0;
}

int
dfu_exec (void) {
    dfu_state_s.dfu_stage = STAGE_EXEC;

    logs(VERBOSITY_INFO, "DFU execution start\r\n");
    while (STAGE_DONE != dfu_state_s.dfu_stage) {
        tud_task();
    }

    /// It is required to call tud_task() again
    logs(VERBOSITY_INFO, "Process USB events from the queue if any\r\n");
    while (tud_task_event_ready()) {
        tud_task();
    }

    /** dfu-util termination workaround
     *  There is no API to get current DFU stage
     */
    logs(VERBOSITY_INFO, "DFU post-execution delay\r\n");
    for (size_t count = USB_POSTPROC_CNT; count; --count) {
        tud_task();
    }

    logs(VERBOSITY_INFO, "DFU post-execution deinit\r\n");
    dfu_deinit();
    
    return 0;
}

//--------------------------------------------------------------------+
// DFU callbacks
// Note: alt is used as the partition number, in order to support multiple
// partitions like FLASH, EEPROM, etc.
//--------------------------------------------------------------------+

/** Invoked right before tud_dfu_download_cb() (state=DFU_DNBUSY) or
 * tud_dfu_manifest_cb() (state=DFU_MANIFEST)
 * Application return timeout in milliseconds (bwPollTimeout) for the next
 * download/manifest operation.
 * During this period, USB host won't try to communicate with us.
 */
uint32_t
tud_dfu_get_timeout_cb (uint8_t alt, uint8_t state) {
    mem_dst_e mem_dst = alt;
    uint32_t retval = 0;

    logs(VERBOSITY_DBG, "DFU get tout alt=%u\r\n", alt);

    if  (state == DFU_DNBUSY) {
        switch (mem_dst) {
            case DST_TCM:
                retval = DFU_TIMEOUT_TCM;
                break;

            case DST_EFLASH:
                retval = DFU_TIMEOUT_EFLASH;
                break;
            
            case DST_QSPIFLASH:
                retval = DFU_TIMEOUT_QSPIFLASH;
                break;
            
            default:
                retval = 10;
                break;
        }
    
    } else if (state == DFU_MANIFEST) {
        /// since we don't buffer entire image
        retval = 0;
    }

    return retval;
}

/** Invoked when received DFU_DNLOAD (wLength>0) following by DFU_GETSTATUS
 * (state=DFU_DNBUSY) requests
 * This callback could be returned before flashing op is complete (async).
 * Once finished flashing, application must call tud_dfu_finish_flashing()
 */
void
tud_dfu_download_cb (uint8_t alt,
                     uint16_t block_num,
                     uint8_t const* data,
                     uint16_t length) {
    mem_dst_e mem_dst  = alt;
    uint32_t tail_word = 0;
    uint16_t data_cnt  = 0;
    uint32_t *data_ptr = NULL;
    uint8_t mem_wr_sz  = sizeof(uint32_t);

    logs(VERBOSITY_DBG, "DFU dload callback: alt = %d, block_num = %d, " \
                        "length = %d\r\n", alt, block_num, length);

    /// Erase eFlash if required
    if ((DST_EFLASH == mem_dst) &&
        (false == dfu_state_s.eflash_erased)) {
        logs(VERBOSITY_DBG, "eFlash init: ");
        logs(VERBOSITY_DBG,"done\r\n");
        logs(VERBOSITY_DBG, "DFU dload: eFlash erase\r\n");
#if defined (BE_U1000)
        CRU_EFLASH_DMAMode_Enable();
#endif
        EFLASH_EraseAll(EFLASH_MAIN_ARRAY);
        dfu_state_s.eflash_erased = true;
    }

    /// Init and erase QSPI flash if required
    if ((DST_QSPIFLASH == mem_dst) &&
        (false == dfu_state_s.qspiflash_erased)) {
        logs(VERBOSITY_DBG, "DFU dload: QSPI\r\n");
        qspi_init();
        logs(VERBOSITY_DBG, "Erase QSPI flash\r\n");
        qspi_flash_erase();
        dfu_state_s.qspiflash_erased = true;
        logs(VERBOSITY_DBG, "QSPI interface: set Quad mode\r\n");
        qspi_flash_set_quad(ENABLE);
    }

    /// Write data to alt-defined memory region
    switch (mem_dst) {
        case DST_TCM:
            memcpy((uint8_t *)(BASE_ADDR_TCM + dfu_state_s.dnload_cnt[mem_dst]),
                    data, length);
            dfu_state_s.dnload_cnt[mem_dst] += length;
            break;

        case DST_EFLASH:
            while (data_cnt < length) {
                /// Not the last data, or length is a multiple of the word size
                if ((length - data_cnt) >= (uint16_t)sizeof(uint32_t)) {
                    data_ptr = (uint32_t *)(data + data_cnt);

                /// The data tail, when length is not multiple of the word size
                } else {
                    data_ptr = &tail_word;
                    mem_wr_sz = length - data_cnt;
                    memcpy((uint8_t *)&tail_word, data, mem_wr_sz);
                }

                EFLASH_ProgramWord(BASE_ADDR_EFLASH + dfu_state_s.dnload_cnt[mem_dst],
                    *data_ptr, EFLASH_MAIN_ARRAY);
                dfu_state_s.dnload_cnt[mem_dst] += mem_wr_sz;
                data_cnt += mem_wr_sz;
            }
            
            break;

        case DST_QSPIFLASH:
            qspi_flash_write(dfu_state_s.dnload_cnt[mem_dst], data, length);
            dfu_state_s.dnload_cnt[mem_dst] += length;
            break;

        default:
            break;
    }

    // flashing op for download complete without error
    tud_dfu_finish_flashing(DFU_STATUS_OK);
}

/** Invoked when received DFU_UPLOAD request
 * Application must populate data with up to length bytes and
 * Return the number of written bytes
 */
uint16_t
tud_dfu_upload_cb(uint8_t alt,
                  uint16_t block_num,
                  uint8_t* data,
                  uint16_t length) {
    mem_dst_e mem_dst = alt;
    uint32_t xfer_len = 0;

    logs(VERBOSITY_DBG, "DFU UPLoad alt=%u block=%u len=%u\r\n",
                         alt, block_num, length);

    switch (mem_dst) {
        case DST_TCM:
            xfer_len = ((MEM_SIZE_TCM - dfu_state_s.upload_cnt[mem_dst]) >= length) ? \
            length : (MEM_SIZE_TCM - dfu_state_s.upload_cnt[mem_dst]);

            memcpy(data, (uint8_t *)(BASE_ADDR_TCM + dfu_state_s.upload_cnt[mem_dst]), xfer_len);
            dfu_state_s.upload_cnt[mem_dst] += xfer_len;
            break;

        case DST_EFLASH:
            xfer_len = ((MEM_SIZE_EFLASH - dfu_state_s.upload_cnt[mem_dst]) >= length) ? \
            length : (MEM_SIZE_EFLASH - dfu_state_s.upload_cnt[mem_dst]);
            
            xfer_len &= ~(sizeof(uint32_t) - 1);
            
            for (uint32_t i = 0; i < xfer_len; i += sizeof(uint32_t)) {
                EFLASH_ReadWord(BASE_ADDR_EFLASH + dfu_state_s.upload_cnt[mem_dst],
                               (uint32_t *)(data + i), EFLASH_MAIN_ARRAY);
                dfu_state_s.upload_cnt[mem_dst] += sizeof (uint32_t);
            }
            break;
        
        case DST_QSPIFLASH:
            xfer_len = ((MEM_SIZE_QSPIFLASH - dfu_state_s.upload_cnt[mem_dst]) >= length) ? \
            length : (MEM_SIZE_QSPIFLASH - dfu_state_s.upload_cnt[mem_dst]);
            qspi_flash_read(dfu_state_s.upload_cnt[mem_dst], data, xfer_len);
            dfu_state_s.upload_cnt[mem_dst] += xfer_len;
            break;

        default:
            break;
    }

    if (xfer_len < length) {
        dfu_state_s.dfu_stage = STAGE_DONE;
    }
    
    return xfer_len;
}

/** Invoked when download process is complete, received DFU_DNLOAD (wLength=0)
 * following by DFU_GETSTATUS (state=Manifest)
 * Application can do checksum, or actual flashing if buffered entire
 * image previously.
 * Once finished flashing, application must call tud_dfu_finish_flashing()
 */
void
tud_dfu_manifest_cb (uint8_t alt) {
    mem_dst_e mem_dst = alt;

    logs(VERBOSITY_INFO, "DFU manifest callback: alt = %d\r\n", alt);
    logs(VERBOSITY_INFO, "Received (bytes): %d\r\n",
                          dfu_state_s.dnload_cnt[mem_dst]);
    dfu_state_s.dfu_stage = STAGE_DONE;

    if (DST_QSPIFLASH == mem_dst) {
        dfu_state_s.qspiflash_erased = false;
        qspi_flash_set_quad(DISABLE);
    }

    /** Flashing op for manifest is complete without error
     * Application can perform checksum, should it fail, use appropriate
     * status such as errVERIFY.
     */
    tud_dfu_finish_flashing(DFU_STATUS_OK);
}

/// Invoked when the Host has terminated a download or upload transfer
void
tud_dfu_abort_cb(uint8_t __attribute__((__unused__))alt) {
    logs(VERBOSITY_INFO, "DFU abort callback: alt = %d\r\n", alt);
    dfu_deinit();
}

/// Invoked when a DFU_DETACH request is received
void
tud_dfu_detach_cb(void) {
    logs(VERBOSITY_INFO, "DFU detach callback\r\n");
    dfu_deinit();
}
