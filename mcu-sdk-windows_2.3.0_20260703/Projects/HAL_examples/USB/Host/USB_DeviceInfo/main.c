/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/USB/Host/USB_DeviceInfo/main.c
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
#include <string.h>
#include <iconv.h>
#include "bmcu_common.h"
#include "bmcu_cru.h"
#include "bmcu_eflash.h"
#include "tusb.h"
#include "bsp.h"

#define LANGID          0x0409U /* English */

/* Device state */
enum {
    STATE_NOT_MOUNTED = 0,
    STATE_MOUNTED
};

CFG_TUH_MEM_ALIGN tusb_desc_device_t device_descriptor;
CFG_TUH_MEM_ALIGN uint8_t serial_string[CFG_TUH_ENUMERATION_BUFSIZE];
CFG_TUH_MEM_ALIGN uint8_t manufacturer_string[CFG_TUH_ENUMERATION_BUFSIZE];
CFG_TUH_MEM_ALIGN uint8_t product_string[CFG_TUH_ENUMERATION_BUFSIZE];

int
__io_putchar (int ch) {
    return bsp_serial_putchar(ch);
}

static int32_t device_state;

static int utf16le_to_utf8 (uint16_t* inbuf,
                            size_t inbytesleft,
                            uint8_t* outbuf,
                            size_t outbytesleft);

static void print_string_desc (uint8_t* desc);

static void led_task(void);

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

    tusb_rhport_init_t host_init = {
        .role = TUSB_ROLE_HOST,
        .speed = TUSB_SPEED_AUTO
    };
    tusb_init(BOARD_TUH_RHPORT, &host_init);

    __enable_irq();
}

int
utf16le_to_utf8 (uint16_t* inbuf,
                 size_t inbytesleft,
                 uint8_t* outbuf,
                 size_t outbytesleft)
{
    iconv_t cd = iconv_open("UTF-8", "UTF-16LE");
    if (cd == ((iconv_t)(-1)))
    {
        return -1;
    }

    char *inptr = (char*)inbuf;
    char *outptr = (char*)outbuf;

    size_t res = iconv(cd, &inptr, &inbytesleft, &outptr, &outbytesleft);
    iconv_close(cd);

    if (res == ((size_t)(-1)))
    {
        return -1;
    }

    return 0;
}

void
print_string_desc (uint8_t* desc)
{
    uint8_t desc_len = desc[0];
    uint8_t desc_type = desc[1];

    if ((desc_len != 0U) &&
        (desc_type == TUSB_DESC_STRING))
    {
        uint8_t utf8_str[CFG_TUH_ENUMERATION_BUFSIZE / 2];

        uint16_t* utf16_str_addr = (uint16_t*)&desc[2];
        size_t utf16_str_len = (size_t)(desc_len - 2U);
        size_t utf8_str_len = utf16_str_len / 2U;

        if (utf16le_to_utf8(utf16_str_addr, utf16_str_len, utf8_str, utf8_str_len) == 0)
        {
            utf8_str[utf8_str_len] = '\0';
            printf("%s", (char*)utf8_str);
        }
    }    
}

void
tuh_mount_cb (uint8_t daddr) {
    tusb_xfer_result_t xfer_result;

    device_state = STATE_MOUNTED;

    /* Get Device Descriptor */
    xfer_result = tuh_descriptor_get_device_sync(daddr, (void*)&device_descriptor, sizeof(tusb_desc_device_t));
    if (xfer_result != XFER_RESULT_SUCCESS)
    {
        printf("Failed to get device descriptor\r\n");
        return;
    }

    /* Print device information */
    printf("Device %u: ID %04x:%04x\r\n", daddr, device_descriptor.idVendor, device_descriptor.idProduct);

    printf("Device Descriptor:\r\n");
    printf("  bLength               %u\r\n", device_descriptor.bLength);
    printf("  bDescriptorType       %u\r\n", device_descriptor.bDescriptorType);
    printf("  bcdUSB                %04x\r\n", device_descriptor.bcdUSB);
    printf("  bDeviceClass          %u\r\n", device_descriptor.bDeviceClass);
    printf("  bDeviceSubClass       %u\r\n", device_descriptor.bDeviceSubClass);
    printf("  bDeviceProtocol       %u\r\n", device_descriptor.bDeviceProtocol);
    printf("  bMaxPacketSize0       %u\r\n", device_descriptor.bMaxPacketSize0);
    printf("  idVendor              %04x\r\n", device_descriptor.idVendor);
    printf("  idProduct             %04x\r\n", device_descriptor.idProduct);
    printf("  bcdDevice             %04x\r\n", device_descriptor.bcdDevice);

    /* Manufacturer String Descriptor */
    printf("  iManufacturer         %u  ", device_descriptor.iManufacturer);
    if (device_descriptor.iManufacturer != 0U)
    {
        xfer_result = tuh_descriptor_get_manufacturer_string_sync(daddr, LANGID, (void*)manufacturer_string, sizeof(manufacturer_string));
        if (xfer_result == XFER_RESULT_SUCCESS)
        {
            print_string_desc(manufacturer_string);
        }
    }
    printf("\r\n");

    /* Product String Descriptor */
    printf("  iProduct              %u  ", device_descriptor.iProduct);
    if (device_descriptor.iProduct != 0U)
    {
        xfer_result = tuh_descriptor_get_product_string_sync(daddr, LANGID, (void*)product_string, sizeof(product_string));
        if (xfer_result == XFER_RESULT_SUCCESS)
        {
            print_string_desc(product_string);
        }
    }
    printf("\r\n");

    /* Serial String Descriptor */
    printf("  iSerialNumber         %u  ", device_descriptor.iSerialNumber);
    if (device_descriptor.iSerialNumber != 0U)
    {
        xfer_result = tuh_descriptor_get_serial_string_sync(daddr, LANGID, (void*)serial_string, sizeof(serial_string));
        if (xfer_result != XFER_RESULT_SUCCESS)
        {
            printf("n/a");
        }
        else
        {
            print_string_desc(serial_string);
        }
    }
    printf("\r\n");

    printf("  bNumConfigurations    %u\r\n", device_descriptor.bNumConfigurations);
}

void
tuh_umount_cb (uint8_t daddr) {
    device_state = STATE_NOT_MOUNTED;

    printf("Device removed, address = %d\r\n", daddr);
}

void
led_task (void) {
    switch(device_state)
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

int
main (void) {
    init();

    printf("USB Host Device Info example, %s %s\r\n", __DATE__, __TIME__);

    for (;;) {
        tuh_task();
        led_task();
    }

	return 0;
}
