/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/USB/Host/USB_MSC/main.c
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

#include <stdint.h>
#include <stdio.h>
#include "bmcu_common.h"
#include "bmcu_cru.h"
#include "bmcu_eflash.h"
#include "tusb.h"
#include "bsp.h"
#include "ff.h"
#include "diskio.h"

/* Device state */
enum {
    STATE_NOT_MOUNTED = 0,
    STATE_MOUNTED
};

#define DISK_DRIVE_PATH_LEN     5U

static FATFS fatfs[CFG_TUH_DEVICE_MAX];                             /* Filesystem object structure */
static TCHAR drive_path[CFG_TUH_DEVICE_MAX][DISK_DRIVE_PATH_LEN];   /* Logical drive number */

static const TCHAR *test_fname = "readme.txt";

static const char *test_file = "# This is the contents of readme.txt\r\n"
                               "# file found on the USB drive. When a \r\n"
                               "# FAT formatted USB drive is attached\r\n"
                               "# and file system is mounted, the test\r\n"
                               "# application searches for the file\r\n"
                               "# named readme.txt in the root folder.\r\n"
                               "# If the file is found, the contents\r\n"
                               "# is printed out to terminal. If not,\r\n"
                               "# the default file with this contents\r\n"
                               "# will be created. Replace this text\r\n"
                               "# with you own data and re-attach USB\r\n"
                               "# drive to see it in terminal.\r\n";

/* SCSI command response */
CFG_TUH_MEM_SECTION static struct {
    TUH_EPBUF_TYPE_DEF(scsi_inquiry_resp_t, inquiry);
} scsi_resp;

int
__io_putchar (int ch) {
    return bsp_serial_putchar(ch);
}

static int32_t device_state;
static bool run_once;

static bool inquiry_complete_cb(uint8_t dev_addr, tuh_msc_complete_data_t const* cb_data);

static void led_task(void);
static void msc_task(void);

/**
 * @brief Clock configuration:
 *
 * PLL   = 200MHz
 * CCLK  = 100MHz
 * PCLK0 = 100MHz
 * PCLK1 = 100MHz
 * PCLK2 = 100MHz
 * HCLK  = 100MHz
 */
void
CLK_Init (void) {

    CRU_PLL_BypassMode_Disable();

    CRU_PLL_SetSource(CRU_PLL_SRC_C0);

    CRU_PLL_ForceReset();
    CRU_PLL_Config(1UL, 48UL, 6UL, 48UL);
    CRU_PLL_ReleaseReset();

    __delay_cycles(500UL);

    CRU_SetCCLKPrescaler(CRU_CLK_DIV_2);
    CRU_CCLKPrescaler_Enable();
    CRU_SetCCLKSource(CRU_CLK_SRC_PLL);

    CRU_SetPCLK0Prescaler(CRU_CLK_DIV_2);
    CRU_PCLK0Prescaler_Enable();
    CRU_SetPCLK0Source(CRU_CLK_SRC_PLL);

    CRU_SetPCLK1Prescaler(CRU_CLK_DIV_2);
    CRU_PCLK1Prescaler_Enable();
    CRU_SetPCLK1Source(CRU_CLK_SRC_PLL);

    CRU_SetPCLK2Prescaler(CRU_CLK_DIV_2);
    CRU_PCLK2Prescaler_Enable();
    CRU_SetPCLK2Source(CRU_CLK_SRC_PLL);

    CRU_SetHCLKPrescaler(CRU_CLK_DIV_2);
    CRU_HCLKPrescaler_Enable();
    CRU_SetHCLKSource(CRU_CLK_SRC_PLL);

    CRU_SetCCLK1MHzPrescaler(99UL);
    CRU_SetHCLK1MHzPrescaler(99UL);

    CRU_USB_PHY_PLL_Enable();
    __delay_cycles(100UL);
    CRU_USB_PHY_ReleaseReset();
    __delay_us(500UL);
}

void
init (void) {
    /* Prepare eFlash to 100 MHz CCLK. See PLL configuration */
    EFLASH_Init(100000000UL);

    CLK_Init();

    CLIC_Config(1U, 1U);
    CLIC_SetLevelThreshold(0U);

    bsp_led_init();

    bsp_serial_init();
    setbuf(stdout, NULL);

    /* USB Host */
    device_state = STATE_NOT_MOUNTED;
    run_once = false;

    tusb_rhport_init_t host_init = {
        .role = TUSB_ROLE_HOST,
        .speed = TUSB_SPEED_AUTO
    };
    tusb_init(BOARD_TUH_RHPORT, &host_init);

    __enable_irq();
}

void
tuh_mount_cb (uint8_t daddr) {
    device_state = STATE_MOUNTED;

    printf("A mass storage device is attached\r\n");

    uint8_t lun = 0U;
    tuh_msc_inquiry(daddr, lun, &scsi_resp.inquiry, inquiry_complete_cb, 0UL);
}

void
tuh_umount_cb (uint8_t daddr) {
    device_state = STATE_NOT_MOUNTED;

    printf("A mass storage device is removed\r\n");

    /* Unmount drive */
    uint8_t drive_num = daddr - 1U;
    f_unmount(drive_path[drive_num]);
}

bool
inquiry_complete_cb (uint8_t dev_addr, tuh_msc_complete_data_t const* cb_data) {
    bool retval = true;

    msc_cbw_t const* cbw = cb_data->cbw;
    msc_csw_t const* csw = cb_data->csw;

    if (csw->status == 0)
    {
        /* Inquiry command succeeded */

        /* Print device info */
        printf("Device:       %.8s %.16s\r\n", scsi_resp.inquiry.vendor_id, scsi_resp.inquiry.product_id);

        /* Print capacity */
        uint32_t block_count = tuh_msc_get_block_count(dev_addr, cbw->lun);
        uint32_t block_size = tuh_msc_get_block_size(dev_addr, cbw->lun);

        printf("Capacity:     %luMB\r\n", block_count / ((1024U * 1024U) / block_size));

        /* Get local drive number */
        uint8_t drive_num = dev_addr - 1U;
        snprintf(drive_path[drive_num], DISK_DRIVE_PATH_LEN, "%u:", drive_num);
        printf("Drive letter: %s\r\n", drive_path[drive_num]);

        /* Mount the logical drive */
        FRESULT res = f_mount(&fatfs[drive_num], drive_path[drive_num], 1U);
        if (res == FR_OK)
        {
            /* Change current directory to the newly mounted drive */
            f_chdir(drive_path[drive_num]);

            /* Get the volume label  */
            TCHAR label[34];
            if (f_getlabel(drive_path[drive_num], label, NULL) == FR_OK)
            {
                printf("Drive label:  %s\r\n", label);
            }

            run_once = true;
        }
        else
        {
            /* Drive mount failed */
            printf("Mount failed\r\n");

            retval = false;
        }
    }
    else
    {
        /* Inquiry command failed */
        retval = false;
    }

    return retval;
}

void
led_task (void) {
    switch (device_state)
    {
        case STATE_MOUNTED:
        {
            bsp_led_on();
        }
        break;

        case STATE_NOT_MOUNTED:
        default:
        {
            bsp_led_off();
        }
        break;
    }
}

void
msc_task (void) {
    static uint8_t buf[512];

    if (run_once)
    {
        FRESULT fr;
        FIL fil;
        FILINFO fno;

        fr = f_stat(test_fname, &fno);
        if (fr == FR_OK)
        {
            /* The file is found: print it */
            printf("%s is found\r\n", test_fname);

            UINT br = 0U;
            if (f_open(&fil, test_fname, FA_READ) == FR_OK)
            {
                printf("\r\n");

                do
                {
                    fr = f_read(&fil, buf, sizeof(buf), &br);

                    for(UINT i = 0; i < br; i++)
                    {
                        if (isprint(buf[i]) || iscntrl(buf[i]))
                        {
                            putchar(buf[i]);
                        }
                    }
                } while ((fr == FR_OK) && (br == sizeof(buf)));

                f_close(&fil);

                printf("\r\n");
            }
        }
        else
        {
            /* The file is not found: create default */
            printf("%s is not found, the default will be created\r\n", test_fname);

            fr = f_open(&fil, test_fname, FA_CREATE_ALWAYS | FA_WRITE);
            if (fr == FR_OK)
            {
                UINT bw;
                f_write(&fil, test_file, strlen(test_file), &bw);
                f_close(&fil);
            }
        }

        run_once = false;
    }
}

int
main (void) {
    init();

    printf("USB Host Mass Storage Class example, %s %s\r\n", __DATE__, __TIME__);

    for (;;) {
        tuh_task();
        led_task();
        msc_task();
    }

	return 0;
}
